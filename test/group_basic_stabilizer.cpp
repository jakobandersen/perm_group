#include "subgroup_util.hpp"

#include <perm_group/group/basic_stabilizer.hpp>

struct Tester {

	Tester(TestProgram &p) : prog(p), sage(prog.verbose) { }

	void noAllocator() { }

	template<typename Maker>
	void withAllocator(Maker maker) {
		using Alloc = typename Maker::type;
		using stab_group_type = pg::basic_stabilizer<Alloc>;
		StabilizerUtil::run<stab_group_type>(prog, sage, maker);
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