#include "util.hpp"
#include "sage.hpp"

#include <perm_group/orbit.hpp>
#include <perm_group/group/generated.hpp>
#include <perm_group/group/io.hpp>
#include <perm_group/permutation/built_in.hpp>
#include <perm_group/permutation/io.hpp>
#include <perm_group/permutation/permutation.hpp>

#include <boost/test/minimal.hpp>

namespace pg = perm_group;

const auto nop = [](auto&&...) {
};

template<template<typename> class Alloc>
void otherTests(perm_group::TestProgram &prog) {
	std::cout << "Other Tests\n" << std::string(80, '-') << std::endl;
	using size_type = std::size_t;
	using perm_type = std::vector<size_type>;
	std::size_t n = 5;
	std::vector<std::string> permStrings = {
		"(0 1)",
		"(4 3 2 1)",
		"(1 2)"
	};
	perm_type p = pg::make_perm<perm_type>(n);
	pg::generated_group<perm_type, Alloc<perm_type> > g(n);
	for(const auto &s : permStrings) {
		pg::read_permutation_cycles(s, p);
		g.add_generator(p);
	}
	pg::write_group(std::cout, g) << std::endl;

	{
		std::vector<size_type> orbit;
		pg::orbit(0, g, pg::make_orbit_callback_output_iterator(std::back_inserter(orbit)));
		std::cout << "Orbit:";
		for(auto o : orbit) std::cout << " " << o;
		std::cout << std::endl;
	}
	{
		pg::Orbit<size_type> orbit(n, 0);
		auto print = [&](int i) {
			std::cout << "Orbit " << i << ":" << std::endl;
			for(auto o : orbit) std::cout << " " << o;
			std::cout << std::endl;
		};
		print(0);
		auto b = begin(generator_ptrs(g));
		auto e = end(generator_ptrs(g));
		orbit.update(b, b, b + 1, nop, nop);
		print(1);
		orbit.update(b, b + 1, b + 2, nop, nop);
		print(2);
		orbit.update(b, b + 2, b + 3, nop, nop);
		print(3);
		orbit.update(b, b + 3, e, nop, nop);
		print(4);
	}
}

using perm_group::TestProgram;

struct Tester {

	Tester(TestProgram &p) : prog(p), sage(prog.verbose) { }

	void noAllocator() { }

	template<template<typename> class Alloc>
	void withAllocator() {
		if(prog.verbose > 0) otherTests<Alloc>(prog);

		for(std::size_t round = 1; round <= prog.rounds; ++round) {
			if(prog.verbose > 0) {
				std::cout << "Round: " << round << "\n";
				sage.printRound(round);
			}
			pg::generated_group<TestProgram::perm_type, Alloc<TestProgram::perm_type> > g(prog.degree);
			for(std::size_t i = 0; i < prog.gensize; ++i) g.add_generator(prog.makeRandomPermutation());
			if(prog.verbose > 0) pg::write_group(std::cout, g) << std::endl;

			sage.printTest("Complete Orbit Calculation");
			sage.loadGroup(g, "g");
			for(std::size_t i = 0; i < prog.degree; ++i) {
				std::vector<TestProgram::size_type> orbit;
				pg::orbit(i, g, pg::make_orbit_callback_output_iterator(std::back_inserter(orbit)));
				if(prog.verbose > 0) {
					std::cout << "Orbit(" << i << "): ";
					std::copy(orbit.begin(), orbit.end(), std::ostream_iterator<TestProgram::size_type>(std::cout, " "));
					std::cout << std::endl;
				}
				sage.checkOrbit("g", i, orbit);
			}

			sage.printTest("Partial Orbit Calculation");
			const auto gens = generator_ptrs(g);
			const auto first = gens.begin();
			for(std::size_t step = 1; step < gens.size(); ++step) {
				for(std::size_t i = 0; i < degree(g); ++i) {
					pg::Orbit<TestProgram::size_type> orbit(degree(g), i);
					for(std::size_t partialEnd = step; partialEnd < gens.size(); partialEnd += step) {
						orbit.update(first, first + partialEnd - step, first + partialEnd, nop, nop);
						sage.loadGroup(first, first + partialEnd, degree(g), "g");
						sage.checkOrbit("g", i, orbit);
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