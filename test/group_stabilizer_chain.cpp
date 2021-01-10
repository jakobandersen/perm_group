#include "subgroup_util.hpp"

#include <perm_group/group/stabilizer_chain.hpp>
#include <perm_group/transversal/explicit.hpp>

struct Tester {

	Tester(TestProgram &p) : prog(p), sage(prog.verbose) { }

	using Alloc = pg::raw_ptr_allocator<std::vector<int> >;
	using transversal = pg::transversal_explicit<Alloc>;
	using stab_group_type = pg::stabilizer_chain<transversal>;

	struct DupChecker {

		DupChecker(const stab_group_type *owner) : owner(owner) { }

		template<typename Iter>
		bool operator()(Iter first, Iter last, const std::vector<int> &p) const {
			return owner->is_member(p);
		}
	public:
		const stab_group_type *owner;
	};

	void noAllocator() {
		Alloc alloc(prog.degree);

		stab_group_type chain(0, alloc);
		pg::generated_group<Alloc, DupChecker> g(alloc, DupChecker(&chain));
		for(int i = 0; i != 10; ++i) {
			auto p = prog.makeRandomPermutation();
			pg::write_permutation_cycles(std::cout << "g.add(", p) << ")" << std::endl;
			g.add_generator(std::move(p), [&](auto first, auto lastOld, auto lastNew) {
				pg::write_permutation_cycles(std::cout << "chain.add(", **lastOld) << ")" << std::endl;
				chain.add_generators(first, lastOld, lastNew);
			});
			std::string indent;
			pg::write_group(std::cout << "g = ", g) << std::endl;
			for(const auto *stab = &chain; stab; stab = stab->get_next()) {
				indent += "  ";
				const auto base = stab->fixed_element();
				const auto &trans = stab->transversal();
				const auto &orbit = trans.orbit();
				std::cout << indent << "orbit(" << base << ") = ";
				std::copy(orbit.begin(), orbit.end(), std::ostream_iterator<int>(std::cout, " "));
				std::cout << std::endl;
				std::cout << indent << "trans(" << base << ") =" << std::endl;
				for(const auto o : orbit) {
					std::cout << indent << "  " << trans.predecessor(o) << " -> " << o << ": ";
					pg::write_permutation_cycles(std::cout, trans.from_element(o)) << std::endl;
				}
				std::cout << indent << "ptr = " << stab << std::endl;
				pg::write_group(std::cout << indent << "stab(" << base << ") = ", *stab) << std::endl;
			}
		}
	}

	template<typename Maker>
	void withAllocator(Maker maker) {
		using Alloc = typename Maker::type;
		using transversal = pg::transversal_explicit<Alloc>;
		using stab_group_type = pg::stabilizer_chain<transversal>;
		//		StabilizerUtil::run<stab_group_type>(prog, sage, maker);

		//		using perm_type = TestProgram::perm_type;
		//
		//		for(std::size_t round = 1; round <= prog.rounds; ++round) {
		//			if(prog.verbose > 0) {
		//				std::cout << "Round: " << round << "\n";
		//				sage.printRound(round);
		//			}
		//
		//			sage.print()
		//					<< "def onCheckSubgroupError(sub, super):\n"
		//					<< "\tif 'stabChain' in globals():\n"
		//					<< "\t\tprint('StabChain:')\n"
		//					<< "\t\tfor a in stabChain:\n"
		//					<< "\t\t\tprint('\t%s' % str(a))\n"
		//					<< "\tprint('StabChainAll:')\n"
		//					<< "\tfor a in stabChainAll:\n"
		//					<< "\t\tprint('\t%s' % str(a))\n"
		//					<< "\tprint('StabChainReal:')\n"
		//					<< "\tfor a in stabChainReal:\n"
		//					<< "\t\tprint('\t%s' % str(a))\n";
		//
		//			// make random permutations of the base points
		//			for(std::size_t subRound = 1; subRound <= prog.subRounds; ++subRound) {
		//				if(prog.verbose > 0) {
		//					std::cout << "Sub-round: " << subRound << "\n";
		//					sage.printSubRound(subRound);
		//				}
		//				std::vector<std::size_t> basePoints(prog.degree);
		//				for(std::size_t i = 0; i < prog.degree; ++i)
		//					basePoints[i] = i;
		//				std::shuffle(basePoints.begin(), basePoints.end(), prog.engine);
		//				if(prog.verbose > 1) {
		//					sage.print() << "print('Base Points:";
		//					for(const auto &bp : basePoints) sage.print() << " " << bp;
		//					sage.print() << "')\n";
		//				}
		//
		//				group stabChainAll(prog.degree);
		//				sage.print() << "g_base = []\n";
		//				for(std::size_t i = 0; i < prog.gensize; ++i) {
		//					auto p = prog.makeRandomPermutation();
		//					sage.print() << "g_base.append(" << sage.perm(p, prog.degree) << ")\n";
		//					if(prog.verbose > 0) pg::write_permutation_cycles(std::cout, p) << std::endl;
		//					stabChainAll.add_generator(std::move(p));
		//
		//				}
		//				//				for(auto p : basePoints) {
		//				//					if(stabChainAll.empty())
		//				//						stabChainAll.emplace_back(p, g);
		//				//					else
		//				//						stabChainAll.emplace_back(p, stabChainAll.back());
		//				//					sage.loadGroup(stabChainAll.back(), "g");
		//				//					sage.print()
		//				//							<< "stabChainAll.append(g)\n"
		//				//							<< "g_stab_real = stabChainReal[-1].stabilizer(" << p << ")\n"
		//				//							<< "stabChainReal.append(g_stab_real)\n";
		//				//					sage.checkSubgroup("g", "g_stab_real", "onCheckSubgroupError");
		//				//					if(isAccurate)
		//				//						sage.checkSubgroup("g_stab_real", "g", "onCheckSubgroupError");
		//				//				}
		//				//				base_group_type<Alloc> gUpdate(prog.degree);
		//				//				std::list<StabGroup> stabChain;
		//				//				for(auto p : basePoints) {
		//				//					if(stabChain.empty())
		//				//						stabChain.emplace_back(p, gUpdate);
		//				//					else
		//				//						stabChain.emplace_back(p, stabChain.back());
		//				//				}
		//				//				const auto gPtrs = generator_ptrs(g);
		//				//				for(auto next = gPtrs.begin(); next != gPtrs.end(); ++next) {
		//				//					const auto gPtr = *next;
		//				//					gUpdate.add_generator(*gPtr);
		//				//					stabChain.back().update();
		//				//					sage.print() << "stabChain = [g_base]\n";
		//				//					for(auto &g : stabChain) {
		//				//						sage.loadGroup(g, "stab");
		//				//						sage.print() << "stabChain.append(stab)\n";
		//				//					}
		//				//					sage.loadGroup(gPtrs.begin(), next + 1, degree(g), "g_real_partial");
		//				//					sage.print() << "stabChainReal = [g_real_partial]\n";
		//				//					for(const auto &bp : basePoints) {
		//				//						sage.print()
		//				//								<< "g_stab_real = stabChainReal[-1].stabilizer(" << bp << ")\n"
		//				//								<< "stabChainReal.append(g_stab_real)\n";
		//				//					}
		//				//					sage.print() << "stabChainReal[0] = g_base\n";
		//				//					for(int i = 0; i < stabChain.size(); ++i) {
		//				//						sage.checkSubgroup(
		//				//								"stabChain[" + std::to_string(i) + "]",
		//				//								"stabChainAll[" + std::to_string(i) + "]", "onCheckSubgroupError");
		//				//						sage.checkSubgroup(
		//				//								"stabChain[" + std::to_string(i) + "]",
		//				//								"stabChainReal[" + std::to_string(i) + "]", "onCheckSubgroupError");
		//				//						if(isAccurate)
		//				//							sage.checkSubgroup(
		//				//								"stabChainReal[" + std::to_string(i) + "]",
		//				//								"stabChain[" + std::to_string(i) + "]", "onCheckSubgroupError");
		//				//					}
		//				//				}
		//				//				// now they should be isomorphic
		//				//				sage.print() << "stabChain = [g_base]\n";
		//				//				for(auto &g : stabChain) {
		//				//					sage.loadGroup(g, "stab");
		//				//					sage.print() << "stabChain.append(stab)\n";
		//				//				}
		//				//				for(int i = 0; i < stabChain.size(); ++i) {
		//				//					sage.checkSubgroup(
		//				//							"stabChainAll[" + std::to_string(i) + "]",
		//				//							"stabChain[" + std::to_string(i) + "]", "onCheckSubgroupError");
		//				//				}
		//			} // for each sub-round
		//		} // for each round
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