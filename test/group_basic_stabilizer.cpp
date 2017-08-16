#include "stabilizer_util.hpp"

#include <perm_group/group/basic_stabilizer.hpp>

template<template<typename> class Alloc>
void otherTests(perm_group::TestProgram &prog) {
	std::cout << "Other Tests\n" << std::string(80, '-') << std::endl;
	using size_type = std::size_t;
	using perm_type = std::vector<size_type>;
	using base_group_type = pg::generated_group<perm_type, Alloc<perm_type> >;
	using group_type = pg::basic_stabilizer<base_group_type>;
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
	group_type sub_3(3, g);
	group_type sub_3_0(0, sub_3);

	std::vector<size_type> orbit_0;
	pg::orbit(0, g, pg::make_orbit_callback_output_iterator(std::back_inserter(orbit_0)));
	pg::write_group(std::cout << "Group: ", g) << std::endl;
	std::cout << "Orbit of 0:";
	for(auto o : orbit_0) std::cout << " " << o;
	std::cout << std::endl;

	std::vector<size_type> orbit_3_1;
	pg::orbit(1, sub_3, pg::make_orbit_callback_output_iterator(std::back_inserter(orbit_3_1)));
	pg::write_group(std::cout << "Stab 3: ", sub_3) << std::endl;
	std::cout << "Orbit (stab 3) of 1:";
	for(auto o : orbit_3_1) std::cout << " " << o;
	std::cout << std::endl;

	std::vector<size_type> orbit_0_1;
	pg::orbit(1, sub_3_0, pg::make_orbit_callback_output_iterator(std::back_inserter(orbit_0_1)));
	pg::write_group(std::cout << "Stab 3, 0: ", sub_3_0) << std::endl;
	std::cout << "Orbit (stab 3, 0) of 1:";
	for(auto o : orbit_0_1) std::cout << " " << o;
	std::cout << std::endl;
}

using perm_group::TestProgram;

struct Tester {

	Tester(TestProgram &p) : prog(p), sage(prog.verbose) { }

	void noAllocator() { }

	template<template<typename> class Alloc>
	void withAllocator() {
		if(prog.verbose > 0) otherTests<Alloc>(prog);

		using base_group_type = StabilizerUtil::base_group_type<Alloc>;
		using stab_group_type = pg::basic_stabilizer<base_group_type>;
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