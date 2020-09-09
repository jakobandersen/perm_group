#include <perm_group/permutation/array.hpp>
#include <perm_group/permutation/built_in.hpp>
#include <perm_group/permutation/io.hpp>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

namespace pg = perm_group;

using value_type = std::size_t;

template<typename Perm, typename ConstPerm, bool SizeAware>
struct Test {
	BOOST_CONCEPT_ASSERT((pg::Permutation<ConstPerm>));
	BOOST_CONCEPT_ASSERT((pg::MutablePermutation<Perm>));

	Test() {
		std::size_t n = 42;
		Perm p = pg::make_perm<Perm>(n);
		Perm id = pg::make_identity_perm<Perm>(n);
		(void) id;
		test(p, std::integral_constant<bool, SizeAware > ());
	}
private:

	template<typename ...N>
	void test(Perm &p, N ...n) {
		pg::read_permutation_cycles("(0)", p, n...);
		pg::write_permutation_cycles(std::cout << "Id: ", p, n...) << std::endl;
		pg::read_permutation_cycles("(1 3)", p, n...);
		pg::write_permutation_cycles(std::cout << "Simple cycle: ", p, n...) << std::endl;
		pg::read_permutation_cycles("(1 40 30 5 12)(3 6 10)", p, n...);
		pg::write_permutation_cycles(std::cout << "Multiple cycles: ", p, n...) << std::endl;
		// parsing error
		try {
			pg::read_permutation_cycles("nope", p, n...);
			BOOST_ASSERT(false);
		} catch(const perm_group::io_error &e) {
			std::cout << "Parsing error: " << e.what() << std::endl;
		}
		// domain error
		try {
			pg::read_permutation_cycles("(60)", p, n...);
			BOOST_ASSERT(false);
		} catch(const perm_group::io_error &e) {
			std::cout << "Domain error: " << e.what() << std::endl;
		}
		// not a permutation error
		try {
			pg::read_permutation_cycles("(0 1)(0 2)", p, n...);
			BOOST_ASSERT(false);
		} catch(const perm_group::io_error &e) {
			std::cout << "Not a permutation error: " << e.what() << std::endl;
		}
	}

	void test(Perm &p, std::true_type) {
		BOOST_CONCEPT_ASSERT((pg::DegreeAwarePermutation<Perm>));
		test(p);
	}

	void test(Perm &p, std::false_type) {
		std::size_t n = 42;
		test(p, n);
	}
};

BOOST_AUTO_TEST_CASE(test_main) {
	Test<std::vector<value_type>, const std::vector<value_type>, true > ();
	Test<pg::array_permutation<value_type>, const pg::array_permutation<value_type>, false>();
	Test<std::array<value_type, 42>, const std::array<value_type, 42>, true>();
}
