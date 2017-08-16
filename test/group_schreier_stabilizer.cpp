#include "stabilizer_util.hpp"

#include <perm_group/group/schreier_stabilizer.hpp>
#include <perm_group/transversal/explicit.hpp>

struct Tester {

	Tester(TestProgram &p) : prog(p), sage(prog.verbose) { }

	void noAllocator() { }

	template<template<typename> class Alloc>
	void withAllocator() {
		using base_group_type = StabilizerUtil::base_group_type<Alloc>;
		using transversal_type = pg::transversal_explicit<typename base_group_type::allocator>;
		using stab_group_type = pg::schreier_stabilizer<base_group_type, transversal_type>;
		constexpr bool exploreAll = false;
		StabilizerUtil::run<Alloc, stab_group_type>(prog, sage, exploreAll);
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