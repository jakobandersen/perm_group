#ifndef PERM_GROUP_ALLOCATOR_RAW_PTR_HPP
#define PERM_GROUP_ALLOCATOR_RAW_PTR_HPP

#include <perm_group/allocator/allocator.hpp>
#include <perm_group/permutation/permutation.hpp>

namespace perm_group {

// rst: .. class:: template<typename Perm> raw_ptr_allocator
// rst:
// rst:		An `Allocator` than uses ``new`` and ``delete`` for allocating permutations.
// rst:

template<typename Perm>
struct raw_ptr_allocator {
	// rst:		.. type:: perm = Perm
	using perm = Perm;
	// rst:		.. type:: pointer = perm*
	using pointer = perm*;
	// rst:		.. type:: const_pointer = const perm*
	using const_pointer = const perm*;
public:

	// rst:		.. function:: explicit raw_ptr_allocator(std::size_t n)
	// rst:
	// rst:			Construct an allocator that allocates permutations of degree `n`.

	explicit raw_ptr_allocator(std::size_t n) : n(n) { }

	// rst:		.. function:: std::size_t degree() const

	std::size_t degree() const {
		return n;
	}

	// rst:		.. function:: pointer make()
	// rst:
	// rst:			:returns: `new perm(perm_group::make_perm<perm>(degree()))`

	pointer make() {
		return new perm(perm_group::make_perm<perm>(degree()));
	}

	// rst:		.. function:: pointer make_identity()
	// rst:
	// rst:			:returns: `new perm(perm_group::make_identity_perm<perm>(degree()))`

	pointer make_identity() {
		return new perm(perm_group::make_identity_perm<perm>(n));
	}

	// rst:		.. function:: template<typename UPerm> pointer copy(UPerm &&p)
	// rst:
	// rst:			:returns: `new perm(perm_group::copy_perm<perm>(n, std::forward<UPerm>(p)))`

	template<typename UPerm>
	pointer copy(UPerm &&p) {
		return new perm(perm_group::copy_perm<perm>(n, std::forward<UPerm>(p)));
	}

	// rst:		.. function:: void release(const_pointer p)
	// rst:
	// rst:			Does `delete p`.
	
	void release(const_pointer p) {
		delete p;
	}
private:
	std::size_t n;
};

} // namespace perm_group

#endif /* PERM_GROUP_ALLOCATOR_RAW_PTR_HPP */