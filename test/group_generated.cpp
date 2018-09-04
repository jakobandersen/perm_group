#include "util.hpp"

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

	runForEachAllocator<perm_type>([](auto maker) {
		using Maker = decltype(maker);
		using Alloc = typename Maker::type;
		const std::size_t n = 5;
		Alloc alloc = maker(n);
		{
			using group_type = pg::generated_group<Alloc>;
					BOOST_CONCEPT_ASSERT((pg::Group<group_type>));
					std::vector<std::string> permStrings = {
				"(0 1)",
				"(0 1 2 3 4)",
				"(1 2)"
			};
			perm_type p = pg::make_perm<perm_type>(n);
					group_type g(alloc);
			for(const auto &s : permStrings) {
				pg::read_permutation_cycles(s, p);
						g.add_generator(p);
			}
			pg::write_group(std::cout, g) << std::endl;
		}
		{
			using group_type1 = pg::generated_group<Alloc>;
					using group_type2 = pg::generated_group<Alloc, pg::DupCheckerNop>;
					static_assert(std::is_same<group_type1, group_type2>::value, "");
		}
		{
			using group_type = pg::generated_group<Alloc>;
					group_type g(alloc);
					BOOST_ASSERT(g.generators().size() == 1);
					g.add_generator(pg::make_identity_perm<perm_type>(n));
					BOOST_ASSERT(g.generators().size() == 2);
		}
		{
			std::size_t n = 5;
					using group_type = pg::generated_group<Alloc, pg::DupCheckerCompare>;
					group_type g(alloc);
					BOOST_ASSERT(g.generators().size() == 1);
					g.add_generator(pg::make_identity_perm<perm_type>(n));
					BOOST_ASSERT(g.generators().size() == 1);
		}
	});
	return 0;
}