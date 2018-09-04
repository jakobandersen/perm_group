#ifndef PERM_GROUP_ALLOCATOR_ALLOCATOR_HPP
#define PERM_GROUP_ALLOCATOR_ALLOCATOR_HPP

#include <perm_group/permutation/permutation.hpp>

#include <boost/concept_check.hpp>

namespace perm_group {

// rst: .. concept:: template<typename Alloc> Allocator
// rst:
// rst:		Requires `CopyConstructible<Alloc>`.
// rst:
// rst:		.. assoc_types::
// rst:

template<typename Alloc>
struct Allocator {
	BOOST_CONCEPT_ASSERT((boost::CopyConstructible<Alloc>));
	// An allocator is responsible for deallocating all permutations it has made,
	// when it is destructed.
	// TODO: really?

	// rst:		.. type:: perm = typename Alloc::perm
	// rst:
	// rst:			The type of permutations that `Alloc` allocates.
	// rst:
	// rst:			Requires `Permutation<perm>`.
	// rst:
	using perm = typename Alloc::perm;
	BOOST_CONCEPT_ASSERT((Permutation<perm>));
	// rst:		.. type:: pointer = typename Alloc::pointer
	// rst:		          const_pointer = typename Alloc::const_pointer
	// rst:
	// rst:			Pointer-like types returned and accepted by the allocator.
	// rst:
	// rst:			Requires `NullablePointer<pointer>`, `NullablePointer<const_pointer>`.
	// rst:
	using pointer = typename Alloc::pointer;
	using const_pointer = typename Alloc::const_pointer;
	BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<pointer>));
	BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<const_pointer>));

	// rst:		.. notation::
	// rst:
	// rst:		.. var:: Alloc alloc
	// rst:		         const Alloc cAlloc
	// rst:		.. var:: pointer ptr
	// rst:		         const_pointer cPtr
	// rst:		.. var:: const UPerm cp
	// rst:
	// rst:			A `Permutation`.
	// rst:
	// rst:		.. valid_expr::
	// rst:

	BOOST_CONCEPT_USAGE(Allocator) {
		// rst:		- `*ptr`, type `perm&`.
		// rst:		- `*cPtr`, type `const perm&`.
		// rst:		- `cAlloc.degree()`, convertible to an integer type,
		// rst:		  returning the degree of permutations allocated by the allocator.
		// rst:		- `alloc.make()`, returning a `pointer` to a new permutation.
		// rst:		  Its value is unspecified, and may not be safe to read.
		// rst:		- `alloc.make_identity()`, returning a `pointer` to a new permutation
		// rst:		  initialized to be the identity permutation.
		// rst:		- `alloc.copy(cp)`, returning a `pointer` to a new permutation
		// rst:		  initialized to be a copy of the given permutation.
		// rst:		- `alloc.release(cPtr)`, gives a permutation back to the allocator.
		// rst:		  Permutations must be released by the same allocator or a copy of the allocator
		// rst:		  they were allocated by.
		std::size_t n = cAlloc.degree();
		(void) n;
		pointer ptr = alloc.make();
		const_pointer id = alloc.make_identity();
		alloc.release(ptr);
		alloc.release(id);
		const perm &p = *ptr;
		const_pointer cPtr = ptr;
		(void) cPtr;
		const_pointer cpy = alloc.copy(p);
		(void) cpy;
	}
public:
	std::size_t n;
	Alloc alloc;
	const Alloc cAlloc;
};

} // namespace perm_group

#endif /* PERM_GROUP_ALLOCATOR_ALLOCATOR_HPP */