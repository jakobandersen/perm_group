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

#include <boost/test/minimal.hpp>

namespace pg = perm_group;
using perm_group::TestProgram;

struct StabilizerUtil {
	template<template<typename> class Alloc>
	using base_group_type = pg::generated_group<TestProgram::perm_type, Alloc<TestProgram::perm_type > >;

	template<template<typename> class Alloc, typename StabGroup>
	static void run(TestProgram &prog, perm_group::Sage &sage, const bool exploreAll) {
		constexpr bool isAccurate = StabGroup::is_accurate::value;
		for(std::size_t round = 1; round <= prog.rounds; ++round) {
			if(prog.verbose > 0) {
				std::cout << "Round: " << round << "\n";
				sage.printRound(round);
			}
			base_group_type<Alloc> g(prog.degree);
			for(std::size_t i = 0; i < prog.gensize; ++i) g.add_generator(prog.makeRandomPermutation());
			if(prog.verbose > 0) pg::write_group(std::cout, g) << std::endl;

			sage.loadGroup(g, "g_base");
			sage.print()
					<< "def onCheckSubgroupError(sub, super):\n"
					<< "\tprint('StabChain:')\n"
					<< "\tfor a in stabChain:\n"
					<< "\t\tprint('\t%s' % str(a))\n"
					<< "\tprint('StabChainReal:')\n"
					<< "\tfor a in stabChainReal:\n"
					<< "\t\tprint('\t%s' % str(a))\n";

			if(exploreAll) {
				std::list<StabGroup> stabChain;
				std::vector<std::size_t> basePoints;
				sage.print()
						<< "stabChain = [g_base]\n"
						<< "stabChainReal = [g_base]\n";
				std::vector<bool> inBase(prog.degree, false);
				std::function<void(std::size_t) > makeStabs;
				makeStabs = [&](std::size_t p) {
					basePoints.push_back(p);
					inBase[p] = true;
					if(stabChain.empty())
						stabChain.emplace_back(p, g);
					else
						stabChain.emplace_back(p, stabChain.back());
					{
						auto p = sage.print();
						p << "# BasePoints:";
						for(auto bp : basePoints) p << " " << bp;
						p << "\n";
					}
					sage.loadGroup(stabChain.back(), "g_stab");
					sage.print()
							<< "stabChain.append(g_stab)\n"
							<< "g_stab_real = stabChainReal[-1].stabilizer(" << p << ")\n"
							<< "stabChainReal.append(g_stab_real)\n";
					sage.checkSubgroup("g_stab", "g_stab_real", "onCheckSubgroupError");
					if(isAccurate)
						sage.checkSubgroup("g_stab_real", "g_stab", "onCheckSubgroupError");
					// only recurse if we are not at the identity group
					bool recurse = [&]() {
						const auto &stab = stabChain.back();
						if(generators(stab).size() > 1) return true;
						auto &perm = generators(stab)[0];
						for(std::size_t i = 0; i < prog.degree; ++i) {
							if(perm_group::get(perm, i) != i) return true;
						}
						return false;
					}();
					if(recurse) {
						for(std::size_t i = 0; i < prog.degree; ++i) {
							if(inBase[i]) continue;
							makeStabs(i);
						}
					}
					stabChain.pop_back();
					basePoints.pop_back();
					inBase[p] = false;
					sage.print()
							<< "stabChain.pop()\n"
							<< "stabChainReal.pop()\n";
				};
				for(std::size_t i = 0; i < prog.degree; ++i) {
					makeStabs(i);
				}
			} else { // if !EXPLORE_ALL
				// make random permutations of the base points
				for(std::size_t subRound = 1; subRound <= prog.subRounds; ++subRound) {
					if(prog.verbose > 0) {
						std::cout << "Sub-round: " << subRound << "\n";
						sage.printSubRound(subRound);
					}
					std::list<StabGroup> stabChain;
					std::vector<std::size_t> basePoints(prog.degree);
					for(std::size_t i = 0; i < prog.degree; ++i)
						basePoints[i] = i;
					std::shuffle(basePoints.begin(), basePoints.end(), prog.engine);
					sage.print()
							<< "stabChain = [g_base]\n"
							<< "stabChainReal = [g_base]\n";
					for(auto p : basePoints) {
						if(stabChain.empty())
							stabChain.emplace_back(p, g);
						else
							stabChain.emplace_back(p, stabChain.back());
						{
							auto p = sage.print();
							p << "# BasePoints:";
							for(auto bp : basePoints) p << " " << bp;
							p << "\n";
						}
						sage.loadGroup(stabChain.back(), "g_stab");
						sage.print()
								<< "stabChain.append(g_stab)\n"
								<< "g_stab_real = stabChainReal[-1].stabilizer(" << p << ")\n"
								<< "stabChainReal.append(g_stab_real)\n";
						sage.checkSubgroup("g_stab", "g_stab_real", "onCheckSubgroupError");
						if(isAccurate)
							sage.checkSubgroup("g_stab_real", "g_stab", "onCheckSubgroupError");
						// only continue if we don't have the identity group
						const auto &stab = stabChain.back();
						if(generators(stab).size() > 1) continue;
						auto &perm = generators(stab)[0];
						bool recurse = false;
						for(std::size_t i = 0; i < prog.degree; ++i) {
							if(perm_group::get(perm, i) != i) {
								recurse = true;
								break;
							}
						}
						if(!recurse) break;
					} // for each base point
				} // for each sub-round
			} // end if exploreAll
		} // for each round
	}

	template<template<typename> class Alloc, typename StabGroup>
	static void runUpdatable(TestProgram &prog, perm_group::Sage &sage) {
		constexpr bool isAccurate = StabGroup::is_accurate::value;
		for(std::size_t round = 1; round <= prog.rounds; ++round) {
			if(prog.verbose > 0) {
				std::cout << "Round: " << round << "\n";
				sage.printRound(round);
			}

			base_group_type<Alloc> g(prog.degree);
			for(std::size_t i = 0; i < prog.gensize; ++i) g.add_generator(prog.makeRandomPermutation());
			if(prog.verbose > 0) pg::write_group(std::cout, g) << std::endl;

			sage.loadGroup(g, "g_base");
			sage.print()
					<< "def onCheckSubgroupError(sub, super):\n"
					<< "\tif 'stabChain' in globals():\n"
					<< "\t\tprint('StabChain:')\n"
					<< "\t\tfor a in stabChain:\n"
					<< "\t\t\tprint('\t%s' % str(a))\n"
					<< "\tprint('StabChainAll:')\n"
					<< "\tfor a in stabChainAll:\n"
					<< "\t\tprint('\t%s' % str(a))\n"
					<< "\tprint('StabChainReal:')\n"
					<< "\tfor a in stabChainReal:\n"
					<< "\t\tprint('\t%s' % str(a))\n";

			// make random permutations of the base points
			for(std::size_t subRound = 1; subRound <= prog.subRounds; ++subRound) {
				if(prog.verbose > 0) {
					std::cout << "Sub-round: " << subRound << "\n";
					sage.printSubRound(subRound);
				}
				std::vector<std::size_t> basePoints(prog.degree);
				for(std::size_t i = 0; i < prog.degree; ++i)
					basePoints[i] = i;
				std::shuffle(basePoints.begin(), basePoints.end(), prog.engine);
				if(prog.verbose > 1) {
					sage.print() << "print('Base Points:";
					for(const auto &bp : basePoints) sage.print() << " " << bp;
					sage.print() << "')\n";
				}

				std::list<StabGroup> stabChainAll;
				sage.print()
						<< "stabChainAll = [g_base]\n"
						<< "stabChainReal = [g_base]\n";
				for(auto p : basePoints) {
					if(stabChainAll.empty())
						stabChainAll.emplace_back(p, g);
					else
						stabChainAll.emplace_back(p, stabChainAll.back());
					sage.loadGroup(stabChainAll.back(), "g");
					sage.print()
							<< "stabChainAll.append(g)\n"
							<< "g_stab_real = stabChainReal[-1].stabilizer(" << p << ")\n"
							<< "stabChainReal.append(g_stab_real)\n";
					sage.checkSubgroup("g", "g_stab_real", "onCheckSubgroupError");
					if(isAccurate)
						sage.checkSubgroup("g_stab_real", "g", "onCheckSubgroupError");
				}
				base_group_type<Alloc> gUpdate(prog.degree);
				std::list<StabGroup> stabChain;
				for(auto p : basePoints) {
					if(stabChain.empty())
						stabChain.emplace_back(p, gUpdate);
					else
						stabChain.emplace_back(p, stabChain.back());
				}
				const auto gPtrs = generator_ptrs(g);
				for(auto next = gPtrs.begin(); next != gPtrs.end(); ++next) {
					const auto gPtr = *next;
					gUpdate.add_generator(*gPtr);
					stabChain.back().update();
					sage.print() << "stabChain = [g_base]\n";
					for(auto &g : stabChain) {
						sage.loadGroup(g, "stab");
						sage.print() << "stabChain.append(stab)\n";
					}
					sage.loadGroup(gPtrs.begin(), next + 1, degree(g), "g_real_partial");
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