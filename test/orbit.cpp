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

const auto nop = [](auto&&... args) {
};

template<typename Alloc>
void otherTests(TestProgram &prog, Alloc alloc) {
	std::cout << "Other Tests\n" << std::string(80, '-') << std::endl;
	using size_type = std::size_t;
	using perm = typename Alloc::perm;
	std::size_t n = 5;
	std::vector<std::string> permStrings = {
		"(0 1)",
		"(4 3 2 1)",
		"(1 2)"
	};
	perm p = pg::make_perm<perm>(n);
	pg::generated_group<Alloc> g(alloc);
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
		auto b = begin(g.generator_ptrs());
		auto e = end(g.generator_ptrs());
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

struct Tester {

	Tester(TestProgram &p) : prog(p), sage(prog.verbose) { }

	void noAllocator() { }

	template<typename Maker>
	void withAllocator(Maker maker) {
		using Alloc = typename Maker::type;
		Alloc alloc = maker(prog.degree);
		if(prog.verbose > 0) otherTests<Alloc>(prog, alloc);

		for(std::size_t round = 1; round <= prog.rounds; ++round) {
			if(prog.verbose > 0) {
				std::cout << "Round: " << round << "\n";
				sage.printRound(round);
			}
			pg::generated_group<Alloc> g(alloc);
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
			const auto gens = g.generator_ptrs();
			const auto first = gens.begin();
			for(std::size_t step = 1; step < gens.size(); ++step) {
				for(std::size_t i = 0; i < g.degree(); ++i) {
					pg::Orbit<TestProgram::size_type> orbit(g.degree(), i);
					for(std::size_t partialEnd = step; partialEnd < gens.size(); partialEnd += step) {
						orbit.update(first, first + partialEnd - step, first + partialEnd, nop, nop);
						sage.loadGroup(first, first + partialEnd, g.degree(), "g");
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