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

struct Tester {

	Tester(TestProgram &p) : prog(p), sage(prog.verbose) { }

	void noAllocator() { }

	template<typename Maker>
	void withAllocator(Maker maker) {
		using Alloc = typename Maker::type;
		Alloc alloc = maker(prog.degree);
		using group_type = pg::generated_group<Alloc>;
		using transversal_type = pg::transversal_explicit<Alloc>;

		for(std::size_t round = 1; round <= prog.rounds; ++round) {
			if(prog.verbose > 0) {
				std::cout << "Round: " << round << "\n";
				sage.printRound(round);
			}
			group_type g(alloc);
			for(std::size_t i = 0; i < prog.gensize; ++i) g.add_generator(prog.makeRandomPermutation());
			if(prog.verbose > 0) pg::write_group(std::cout, g) << std::endl;

			sage.printTest("Complete Transversal Calculation");
			sage.loadGroup(g, "g");
			for(std::size_t i = 0; i < prog.degree; ++i) {
				transversal_type trans(i, g.get_allocator());
				const auto gens = g.generator_ptrs();
				trans.update(gens.begin(), gens.begin(), gens.end());
				const auto &o = trans.orbit();
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
			const auto gens = g.generator_ptrs();
			const auto first = gens.begin();
			for(std::size_t step = 1; step < gens.size(); ++step) {
				for(std::size_t i = 0; i != g.degree(); ++i) {
					transversal_type trans(i, g.get_allocator());
					for(std::size_t partialEnd = step; partialEnd < gens.size(); partialEnd += step) {
						const auto nop = [](auto&&... args) {
						};
						trans.update(first, first + partialEnd - step, first + partialEnd, nop, nop);
						sage.loadGroup(first, first + partialEnd, g.degree(), "g");
						const auto &o = trans.orbit();
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