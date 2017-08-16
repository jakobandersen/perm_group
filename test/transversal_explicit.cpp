#include "util.hpp"
#include "sage.hpp"

#include <perm_group/orbit.hpp>
#include <perm_group/group/basic_stabilizer.hpp>
#include <perm_group/group/generated.hpp>
#include <perm_group/group/io.hpp>
#include <perm_group/permutation/built_in.hpp>
#include <perm_group/permutation/io.hpp>
#include <perm_group/permutation/permutation.hpp>
#include <perm_group/transversal/explicit.hpp>

#include <boost/test/minimal.hpp>

namespace pg = perm_group;

template<template<typename> class Alloc>
void otherTests(perm_group::TestProgram &prog) {
	using size_type = std::size_t;
	using perm_type = std::vector<size_type>;
	using base_group_type = pg::generated_group<perm_type, Alloc<perm_type> >;
	using group_type = pg::basic_updatable_stabilizer<base_group_type>;
	using transversal_type = pg::transversal_explicit<typename group_type::allocator>;
	std::size_t n = 5;
	std::vector<std::string> permStrings = {
		"(0 1)",
		"(0 1 2 3 4)",
		"(1 2)"
	};
	perm_type p = pg::make_perm<perm_type>(n);
	base_group_type g(n);
	for(const auto &s : permStrings) {
		pg::read_permutation_cycles(s, p);
		g.add_generator(p);
	}
	transversal_type trans_3(n, 3, get_allocator(g));
	trans_3.update(generator_ptrs(g).begin(), generator_ptrs(g).begin(), generator_ptrs(g).end());
	group_type sub_3(3, g);
	std::size_t g_num_gens = generator_ptrs(g).size();

	transversal_type trans_3_0(n, 0, get_allocator(g));
	trans_3_0.update(generator_ptrs(sub_3).begin(), generator_ptrs(sub_3).begin(), generator_ptrs(sub_3).end());
	group_type sub_3_0(0, sub_3);
	std::size_t sub_3_num_gens = generator_ptrs(sub_3).size();


	auto print = [&]() {
		pg::write_group(std::cout << "Group: ", g) << std::endl;
		std::cout << "Orbit of 3:";
		for(auto o : orbit(trans_3)) std::cout << " " << o;
		std::cout << std::endl;
		std::cout << "Transversal of 3:" << std::endl;
		for(auto o : orbit(trans_3))
			pg::write_permutation_cycles(std::cout << "\t" << o << ": " << trans_3.predecessor(o) << ", ", trans_3.from_element(o)) << std::endl;
		pg::write_group(std::cout << "Stab 3: ", sub_3) << std::endl;
		std::cout << std::endl;

		std::cout << "Orbit (stab 3) of 0:";
		for(auto o : orbit(trans_3_0)) std::cout << " " << o;
		std::cout << std::endl;
		std::cout << "Transversal (stab 3) of 0:" << std::endl;
		for(auto o : orbit(trans_3_0))
			pg::write_permutation_cycles(std::cout << "\t" << o << ": " << trans_3_0.predecessor(o) << ", ", trans_3_0.from_element(o)) << std::endl;
		pg::write_group(std::cout << "Stab 3, 0: ", sub_3_0) << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
	};

	std::cout << "Initial:\n";
	print();
	{
		std::cout << "Added at BaseGroup:\n";
		pg::read_permutation_cycles("(1 4)", p);
		g.add_generator(p);
		print();
	}
	{
		std::cout << "Stab 3 updated:\n";
		auto g_gens = generator_ptrs(g);
		sub_3.update();
		trans_3.update(g_gens.begin(), g_gens.begin() + g_num_gens, g_gens.end());
		g_num_gens = g_gens.size();
		print();
	}
	{
		std::cout << "Stab 3, 0 updated:\n";
		auto g_gens = generator_ptrs(sub_3);
		sub_3_0.update();
		trans_3_0.update(g_gens.begin(), g_gens.begin() + sub_3_num_gens, g_gens.end());
		print();
	}
}

using perm_group::TestProgram;

struct Tester {

	Tester(TestProgram &p) : prog(p), sage(prog.verbose) { }

	void noAllocator() { }

	template<template<typename> class Alloc>
	void withAllocator() {
		using group_type = pg::generated_group<TestProgram::perm_type, Alloc<TestProgram::perm_type> >;
		using transversal_type = pg::transversal_explicit<typename group_type::allocator>;
		if(prog.verbose > 0) otherTests<Alloc>(prog);

		for(std::size_t round = 1; round <= prog.rounds; ++round) {
			if(prog.verbose > 0) {
				std::cout << "Round: " << round << "\n";
				sage.printRound(round);
			}
			group_type g(prog.degree);
			for(std::size_t i = 0; i < prog.gensize; ++i) g.add_generator(prog.makeRandomPermutation());
			if(prog.verbose > 0) pg::write_group(std::cout, g) << std::endl;

			sage.printTest("Complete Transversal Calculation");
			sage.loadGroup(g, "g");
			for(std::size_t i = 0; i < prog.degree; ++i) {
				transversal_type trans(prog.degree, i, get_allocator(g));
				const auto gens = generator_ptrs(g);
				trans.update(gens.begin(), gens.begin(), gens.end());
				const auto &o = orbit(trans);
				if(prog.verbose > 0) {
					std::cout << "Orbit(" << i << "): ";
					std::copy(o.begin(), o.end(), std::ostream_iterator<TestProgram::size_type>(std::cout, " "));
					std::cout << std::endl;
				}
				sage.checkOrbit("g", i, o);
				for(const auto j : o) {
					const auto &p = trans.from_element(j);
					const auto pPtr = trans.from_element_as_ptr(j);
					for(std::size_t k = 0; k < prog.degree; ++k)
						BOOST_ASSERT(pg::get(p, k) == pg::get(*pPtr, k));
					BOOST_ASSERT(pg::get(p, i) == j);
					BOOST_ASSERT(pg::get(*pPtr, i) == j);
				}
			}

			sage.printTest("Partial Transversal Calculation");
			const auto gens = generator_ptrs(g);
			const auto first = gens.begin();
			for(std::size_t step = 1; step < gens.size(); ++step) {
				for(std::size_t i = 0; i < degree(g); ++i) {
					transversal_type trans(prog.degree, i, get_allocator(g));
					for(std::size_t partialEnd = step; partialEnd < gens.size(); partialEnd += step) {
						const auto nop = [](auto&&...) {
						};
						trans.update(first, first + partialEnd - step, first + partialEnd, nop, nop);
						sage.loadGroup(first, first + partialEnd, degree(g), "g");
						const auto &o = orbit(trans);
						sage.checkOrbit("g", i, o);
						for(const auto j : o) {
							const auto &p = trans.from_element(j);
							const auto pPtr = trans.from_element_as_ptr(j);
							for(std::size_t k = 0; k < prog.degree; ++k)
								BOOST_ASSERT(pg::get(p, k) == pg::get(*pPtr, k));
							BOOST_ASSERT(pg::get(p, i) == j);
							BOOST_ASSERT(pg::get(*pPtr, i) == j);
						}
					}
				}
			}
		}
	}
private:
	TestProgram &prog;
	perm_group::Sage sage;
};

int test_main(int argc, char **argv) {
	TestProgram prog(argc, argv);
	prog.run(Tester(prog));
	return 0;
}