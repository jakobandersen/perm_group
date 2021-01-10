#ifndef PERM_GROUP_STABILIZER_UTIL_HPP
#define PERM_GROUP_STABILIZER_UTIL_HPP

#include "util.hpp"
#include "sage.hpp"

#include <perm_group/orbit.hpp>
#include <perm_group/group/generated.hpp>
#include <perm_group/group/io.hpp>
#include <perm_group/permutation/built_in.hpp>
#include <perm_group/permutation/io.hpp>
#include <perm_group/permutation/permutation.hpp>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

namespace pg = perm_group;

template<typename Stab>
struct Callback {

	Callback(std::vector<Stab> &chain, int current)
	: chain(chain), current(current) { }

	template<typename Iter>
	void operator()(Iter first, Iter lastOld, Iter lastNew) {
		if(current + 1 == chain.size()) return;
		chain[current + 1].add_generators(first, lastOld, lastNew, Callback(chain, current + 1));
	}
private:
	std::vector<Stab> &chain;
	int current;
};

struct StabilizerUtil {
	template<typename Alloc>
	using base_group_type = pg::generated_group<Alloc>;

	template<typename Stab, typename AllocMaker>
	static void run(TestProgram &prog, perm_group::Sage &sage, AllocMaker allocMaker) {
		BOOST_CONCEPT_ASSERT((pg::Stabilizer<Stab>));
		using allocator = typename Stab::allocator;
		allocator alloc = allocMaker(prog.degree);
		using perm = typename Stab::perm;
		using pointer = typename allocator::pointer;
		using const_pointer = typename allocator::const_pointer;

		constexpr bool isAccurate = Stab::is_accurate::value;
		for(std::size_t round = 1; round <= prog.rounds; ++round) {
			if(prog.verbose > 0) {
				std::cout << "Round: " << round << "\n";
				sage.printRound(round);
			}

			base_group_type<typename Stab::allocator > g(alloc);
			for(std::size_t i = 0; i < prog.gensize; ++i)
				g.add_generator(prog.makeRandomPermutation());
			if(prog.verbose > 0)
				pg::write_group(std::cout, g) << std::endl;

			sage.loadGroup(g, "g_base");
			sage.print()
					<< "def onCheckSubgroupError(sub, super):\n"
					<< "	if 'stabChain' in globals():\n"
					<< "		print('StabChain:')\n"
					<< "		for a in stabChain:\n"
					<< "			print('\t%s' % str(a))\n"
					<< "	print('StabChainAll:')\n"
					<< "	for a in stabChainAll:\n"
					<< "		print('\t%s' % str(a))\n"
					<< "	print('StabChainReal:')\n"
					<< "	for a in stabChainReal:\n"
					<< "		print('\t%s' % str(a))\n";

			// make random permutations of the base points
			for(std::size_t subRound = 1; subRound <= prog.subRounds; ++subRound) {
				if(prog.verbose > 0) {
					std::cout << "Sub-round: " << subRound << "\n";
					sage.printSubRound(subRound);
				}
				std::vector<std::size_t> basePoints(prog.degree);
				for(std::size_t i = 0; i != prog.degree; ++i)
					basePoints[i] = i;
				std::shuffle(basePoints.begin(), basePoints.end(), prog.engine);
				if(prog.verbose > 1) {
					auto s = sage.print();
					s << "print('Base Points:";
					for(const auto &bp : basePoints)
						s << " " << bp;
					s << "')\n";
				}

				std::vector<Stab> stabChainAll;
				sage.print()
						<< "stabChainAll = [g_base]\n"
						<< "stabChainReal = [g_base]\n";
				for(auto p : basePoints) {
					stabChainAll.emplace_back(p, g.get_allocator());
					if(stabChainAll.size() == 1) {
						const auto gens = g.generator_ptrs();
						stabChainAll.back().add_generators(gens.begin(), gens.begin(), gens.end());
					} else {
						const auto gens = stabChainAll[stabChainAll.size() - 2].generator_ptrs();
						stabChainAll.back().add_generators(gens.begin(), gens.begin(), gens.end());
					}
					sage.loadGroup(stabChainAll.back(), "g");
					sage.print()
							<< "stabChainAll.append(g)\n"
							<< "g_stab_real = stabChainReal[-1].stabilizer(" << p << ")\n"
							<< "stabChainReal.append(g_stab_real)\n";
					sage.checkSubgroup("g", "g_stab_real", "onCheckSubgroupError");
					if(isAccurate)
						sage.checkSubgroup("g_stab_real", "g", "onCheckSubgroupError");
				}

				std::vector<Stab> stabChain;
				for(auto p : basePoints)
					stabChain.emplace_back(p, g.get_allocator());

				const auto gPtrs = g.generator_ptrs();
				for(auto next = gPtrs.begin(); next != gPtrs.end(); ++next) {
					Callback<Stab>(stabChain, -1)(gPtrs.begin(), next, next + 1);

					sage.print() << "stabChain = [g_base]\n";
					for(auto &g : stabChain) {
						sage.loadGroup(g, "stab");
						sage.print() << "stabChain.append(stab)\n";
					}
					sage.loadGroup(gPtrs.begin(), next + 1, g.degree(), "g_real_partial");
					sage.print() << "stabChainReal = [g_real_partial]\n";
					for(const auto &bp : basePoints) {
						sage.print()
								<< "g_stab_real = stabChainReal[-1].stabilizer(" << bp << ")\n"
								<< "stabChainReal.append(g_stab_real)\n";
					}
					sage.print() << "stabChainReal[0] = g_base\n";
					for(int i = 0; i < stabChain.size(); ++i) {
						sage.checkSubgroup(
								"stabChain[" + std::to_string(i) + "]",
								"stabChainAll[" + std::to_string(i) + "]", "onCheckSubgroupError");
						sage.checkSubgroup(
								"stabChain[" + std::to_string(i) + "]",
								"stabChainReal[" + std::to_string(i) + "]", "onCheckSubgroupError");
						if(isAccurate)
							sage.checkSubgroup(
								"stabChainReal[" + std::to_string(i) + "]",
								"stabChain[" + std::to_string(i) + "]", "onCheckSubgroupError");
					}
				}
				// now they should be isomorphic
				sage.print() << "stabChain = [g_base]\n";
				for(auto &g : stabChain) {
					sage.loadGroup(g, "stab");
					sage.print() << "stabChain.append(stab)\n";
				}
				for(int i = 0; i < stabChain.size(); ++i) {
					sage.checkSubgroup(
							"stabChainAll[" + std::to_string(i) + "]",
							"stabChain[" + std::to_string(i) + "]", "onCheckSubgroupError");
				}
			} // for each sub-round
		} // for each round
	}
};

#endif /* PERM_GROUP_STABILIZER_UTIL_HPP */