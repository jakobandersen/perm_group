#include "subgroup_util.hpp"

#include <perm_group/group/schreier_stabilizer.hpp>
#include <perm_group/transversal/explicit.hpp>

struct Tester {

	Tester(TestProgram &p) : prog(p), sage(prog.verbose) { }

	void noAllocator() { }

	template<typename Maker>
	void withAllocator(Maker maker) {
		using Alloc = typename Maker::type;
		using transversal = pg::transversal_explicit<Alloc>;
		using stab_group_type = pg::schreier_stabilizer<transversal>;
		StabilizerUtil::run<stab_group_type>(prog, sage, maker);

		using group_type = pg::generated_group<Alloc>;
		for(std::size_t round = 1; round <= prog.rounds; ++round) {
			if(prog.verbose > 0) {
				std::cout << "Round: " << round << "\n";
				sage.printRound(round);
			}
			group_type g(maker(prog.degree));
			for(std::size_t i = 0; i < prog.gensize; ++i) g.add_generator(prog.makeRandomPermutation());
			if(prog.verbose > 0) pg::write_group(std::cout, g) << std::endl;

			sage.printTest("Complete Transversal Calculation");
			sage.loadGroup(g, "g");
			for(std::size_t i = 0; i < prog.degree; ++i) {
				stab_group_type stab(i, g.get_allocator());
				const auto gens = g.generator_ptrs();
				stab.add_generators(gens.begin(), gens.begin(), gens.end());
				for(std::size_t sub = 0; sub != prog.subRounds; ++sub) {
					const auto perm = prog.makeRandomPermutation();
					auto ptr = stab.sift(perm);
					if(ptr) {
						BOOST_ASSERT(pg::get(*ptr, i) == i);
						stab.get_allocator().release(std::move(ptr));
					}
				}
			}
		}
	}
private:
	TestProgram &prog;
	perm_group::Sage sage;
};

BOOST_AUTO_TEST_CASE(test_main) {
	TestProgram prog(boost::unit_test::framework::master_test_suite().argc,
	                 boost::unit_test::framework::master_test_suite().argv);
	prog.run(Tester(prog));
}