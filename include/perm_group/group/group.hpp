#ifndef PERM_GROUP_GROUP_GROUP_HPP
#define PERM_GROUP_GROUP_GROUP_HPP

#include <perm_group/allocator/allocator.hpp>
#include <perm_group/permutation/permutation.hpp>

#include <boost/concept_check.hpp>

namespace perm_group {

// rst: .. concept:: template<typename GroupT> \
// rst:              Group
// rst:
// rst:		.. assoc_types::
// rst:
// rst:		.. type:: perm_type = typename GroupT::perm_type
// rst:		.. type:: allocator = typename GroupT::allocator
// rst:		.. type:: pointer = typename allocator::pointer
// rst:		.. type:: const_pointer = typename allocator::const_pointer
// rst:
// rst:		Requires `Permutation<perm_type>` and `Allocator<allocator>`.
// rst:

template<typename GroupT>
struct Group {
	using perm_type = typename GroupT::perm_type;
	using allocator = typename GroupT::allocator;
	using pointer = typename allocator::pointer;
	using const_pointer = typename allocator::const_pointer;

	BOOST_CONCEPT_ASSERT((Permutation<perm_type>));
	BOOST_CONCEPT_ASSERT((AllocatorConcept<allocator>));

	// rst:		.. notation::
	// rst:
	// rst:		.. var:: GroupT g
	// rst:		.. var:: const GroupT cg
	// rst:		.. var:: std::size_t n
	// rst:
	// rst:		.. valid_expr::
	// rst:
	// rst:		- `n = degree(cg)`, returns the number of elements the group is defined on.
	// rst:		- `generators(cg)`, returns a non-empty random-access range of `perm_type`.
	// rst:		- `generator_ptrs(cg)`, returns a random-access range of pointer-like values to `perm_type`.
	// rst:		  The range and order of the dereferenced pointers must be eaxtly the range returned by `generators(cg)`.
	// rst:		- `get_allocator(cg)`, returns the allocator used by the group.
	BOOST_CONCEPT_USAGE(Group) {
		const GroupT &cGroup = group;
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
	GroupT group;
};

} // namespace perm_group

#endif /* PERM_GROUP_GROUP_GROUP_HPP */