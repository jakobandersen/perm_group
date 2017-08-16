#ifndef PERM_GROUP_ALLOCATOR_ALLOCATOR_HPP
#define PERM_GROUP_ALLOCATOR_ALLOCATOR_HPP

#include <perm_group/permutation/permutation.hpp>

#include <boost/concept_check.hpp>

namespace perm_group {

template<typename Alloc>
struct AllocatorConcept {
	// An allocator is responsible for deallocating all permutations it has made,
	// when it is destructed.
	using perm_type = typename Alloc::perm_type;
	BOOST_CONCEPT_ASSERT((PermutationConcept<perm_type>));
	using pointer = typename Alloc::pointer;
	using const_pointer = typename Alloc::const_pointer;
	BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<pointer>));
	BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<const_pointer>));

	BOOST_CONCEPT_USAGE(AllocatorConcept) {
		pointer ptr = alloc.make(n);
		alloc.release(n, ptr);
		const perm_type &p = *ptr;
		const_pointer cPtr = ptr;
		const_pointer id = alloc.make_identity(n);
		alloc.release(n, id);
		const_pointer cpy = alloc.copy(n, p);
	}
public:
	std::size_t n;
	Alloc alloc;
};

} // namespace perm_group

#endif /* PERM_GROUP_ALLOCATOR_ALLOCATOR_HPP */