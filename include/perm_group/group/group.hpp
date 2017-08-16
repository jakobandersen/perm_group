#ifndef PERM_GROUP_GROUP_GROUP_HPP
#define PERM_GROUP_GROUP_GROUP_HPP

#include <perm_group/allocator/allocator.hpp>
#include <perm_group/permutation/permutation.hpp>

#include <boost/concept_check.hpp>

namespace perm_group {

template<typename Group>
struct GroupConcept {
	using perm_type = typename Group::perm_type;
	using allocator = typename Group::allocator;
	using pointer = typename allocator::pointer;
	using const_pointer = typename allocator::const_pointer;

	BOOST_CONCEPT_ASSERT((PermutationConcept<perm_type>));
	BOOST_CONCEPT_ASSERT((AllocatorConcept<allocator>));

	BOOST_CONCEPT_USAGE(GroupConcept) {
		const Group &cGroup = group;
		using std::begin;
		using std::end;
		std::size_t n = degree(cGroup);
		{
			auto gs = generators(cGroup);
			using iterator = typename decltype(gs)::iterator;
			iterator first = begin(gs);
			iterator last = end(gs);
			const perm_type &p = *first;
		}
		{
			auto gs = generator_ptrs(cGroup);
			using iterator = typename decltype(gs)::iterator;
			iterator first = begin(gs);
			iterator last = end(gs);
			const_pointer ptr = *first;
		}
		allocator &alloc = get_allocator(cGroup);
	}
private:
	Group group;
};

} // namespace perm_group

#endif /* PERM_GROUP_GROUP_GROUP_HPP */