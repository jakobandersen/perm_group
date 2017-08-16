#include <perm_group/group/generated.hpp>
#include <perm_group/group/io.hpp>
#include <perm_group/permutation/built_in.hpp>
#include <perm_group/permutation/io.hpp>
#include <perm_group/permutation/permutation.hpp>

#include <boost/test/minimal.hpp>

namespace pg = perm_group;

int test_main(int argc, char **argv) {
	using size_type = std::size_t;
	using perm_type = std::vector<size_type>;
	using group_type = pg::generated_group<perm_type>;
	BOOST_CONCEPT_ASSERT((pg::GroupConcept<group_type>));
	std::size_t n = 5;
	std::vector<std::string> permStrings = {
		"(0 1)",
		"(0 1 2 3 4)",
		"(1 2)"
	};
	perm_type p = pg::make_perm<perm_type>(n);
	group_type g(n);
	for(const auto &s : permStrings) {
		pg::read_permutation_cycles(s, p);
		g.add_generator(p);
	}
	pg::write_group(std::cout, g) << std::endl;
	return 0;
}