#include <perm_group/permutation/array.hpp>
#include <perm_group/permutation/built_in.hpp>
#include <perm_group/permutation/io.hpp>

#include <boost/test/minimal.hpp>

namespace pg = perm_group;

using value_type = std::size_t;

template<typename Perm, typename ConstPerm, bool SizeAware>
struct Test {
	BOOST_CONCEPT_ASSERT((pg::PermutationConcept<ConstPerm>));
	BOOST_CONCEPT_ASSERT((pg::MutablePermutationConcept<Perm>));

	Test() {
		std::size_t n = 42;
		Perm p = pg::make_perm<Perm>(n);
		Perm id = pg::make_identity_perm<Perm>(n);
		test(p, std::integral_constant<bool, SizeAware > ());
	}
private:

	void test(Perm &p, std::true_type) {
		BOOST_CONCEPT_ASSERT((pg::SizeAwarePermutationConcept<Perm>));
		pg::read_permutation_cycles("(0)", p);
		pg::write_permutation_cycles(std::cout << "Id: ", p) << std::endl;
		pg::read_permutation_cycles("(1 3)", p);
		pg::write_permutation_cycles(std::cout << "Simple cycle: ", p) << std::endl;
		pg::read_permutation_cycles("(1 40 30 5 12)(3 6 10)", p);
		pg::write_permutation_cycles(std::cout << "Multiple cycles: ", p) << std::endl;
	}

	void test(Perm &p, std::false_type) {
		std::size_t n = 42;
		pg::read_permutation_cycles("(0)", p, n);
		pg::write_permutation_cycles(std::cout << "Id: ", p, n) << std::endl;
		pg::read_permutation_cycles("(1 3)", p, n);
		pg::write_permutation_cycles(std::cout << "Simple cycle: ", p, n) << std::endl;
		pg::read_permutation_cycles("(1 40 30 5 12)(3 6 10)", p, n);
		pg::write_permutation_cycles(std::cout << "Multiple cycles: ", p, n) << std::endl;
	}
};

int test_main(int argc, char **argv) {
	Test<std::vector<value_type>, const std::vector<value_type>, true > ();
	Test<pg::array_permutation<value_type>, const pg::array_permutation<value_type>, false>();
	Test<std::array<value_type, 42>, const std::array<value_type, 42>, true>();
	return 0;
}