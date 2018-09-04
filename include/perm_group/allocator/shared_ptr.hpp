#ifndef PERM_GROUP_ALLOCATOR_SHARED_PTR_HPP
#define PERM_GROUP_ALLOCATOR_SHARED_PTR_HPP

#include <perm_group/allocator/allocator.hpp>
#include <perm_group/permutation/permutation.hpp>

#include <cassert>
#include <memory>

namespace perm_group {

// rst: .. class:: template<typename Perm> \
// rst:            shared_ptr_allocator
// rst:
// rst:		An `Allocator` using reference-counting to guarantee no memory leaks,
// rst:		even when `release` is not used correctly.
// rst:

template<typename Perm>
struct shared_ptr_allocator {
	// rst:		.. type:: perm = Perm
	using perm = Perm;
	// rst:		.. type:: pointer = std::shared_ptr<perm>
	using pointer = std::shared_ptr<perm>;
	// rst:		.. type:: const_pointer = std::shared_ptr<const perm>
	using const_pointer = std::shared_ptr<const perm>;

	// rst:		.. function:: explicit shared_ptr_allocator(std::size_t n)
	// rst:
	// rst:			Construct an allocator that allocates permutations of degree `n`.

	explicit shared_ptr_allocator(std::size_t n) : n(n) { }

	// rst:		.. function:: std::size_t degree() const

	std::size_t degree() const {
		return n;
	}

	// rst:		.. function:: pointer make()
	// rst:
	// rst:			:returns: `std::make_shared<perm>(perm_group::make_perm<perm>(degree()))`

	pointer make() {
		return std::make_shared<perm>(perm_group::make_perm<perm>(degree()));
	}

	// rst:		.. function:: pointer make_identity()
	// rst:
	// rst:			:returns: `std::make_shared<perm>(perm_group::make_identity_perm<perm>(degree()))`

	pointer make_identity() {
		return std::make_shared<perm>(perm_group::make_identity_perm<perm>(degree()));
	}

	// rst:		.. function:: template<typename UPerm> pointer copy(UPerm &&p)
	// rst:
	// rst:			:returns: `std::make_shared<perm>(perm_group::copy_perm<perm>(std::forward<UPerm>(p)))`

	template<typename UPerm>
	pointer copy(UPerm &&p) {
		return std::make_shared<perm>(perm_group::copy_perm<perm>(std::forward<UPerm>(p)));
	}

	// rst:		.. function:: void release(const_pointer p)
	// rst:
	// rst:			Does nothing, though at this stage `p` should be the sole reference to the permutation.

	void release(const_pointer p) { }
private:
	std::size_t n;
};

} // namespace perm_group

#endif /* PERM_GROUP_ALLOCATOR_SHARED_PTR_HPP */