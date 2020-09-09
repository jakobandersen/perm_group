#include <perm_group/permutation/built_in.hpp>
#include <perm_group/permutation/io.hpp>
#include <perm_group/permutation/mult.hpp>
#include <perm_group/permutation/permutation.hpp>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

namespace pg = perm_group;

BOOST_AUTO_TEST_CASE(test_main) {
	using size_type = std::size_t;
	using perm_type = std::vector<size_type>;
	std::size_t n = 5;
	perm_type p = pg::make_perm<perm_type>(n);
	pg::read_permutation_cycles("(0 1 2 3 4)", p);
	perm_type pRes = pg::mult(p, p);
	auto res = pg::mult(pg::mult(p, p), p);
	pRes = pg::copy_perm<perm_type>(pg::mult(p, res));
	pg::write_permutation_cycles(std::cout << "Mult: ", pRes);
}