#ifndef PERM_GROUP_ALLOCATOR_DEFAULT_HPP
#define PERM_GROUP_ALLOCATOR_DEFAULT_HPP

#include <perm_group/allocator/allocator.hpp>
#include <perm_group/permutation/permutation.hpp>

#include <cassert>
#include <memory>

namespace perm_group {

template<typename Perm>
struct default_allocator {
	using perm_type = Perm;
	using pointer = std::shared_ptr<Perm>;
	using const_pointer = std::shared_ptr<const Perm>;

	pointer make(std::size_t n) {
		return std::make_shared<Perm>(perm_group::make_perm<Perm>(n));
	}

	pointer make_identity(std::size_t n) {
		return std::make_shared<Perm>(perm_group::make_identity_perm<Perm>(n));
	}

	template<typename UPerm>
	pointer copy(std::size_t n, UPerm &&p) {
		return std::make_shared<Perm>(perm_group::copy_perm<Perm>(n, std::forward<UPerm>(p)));
	}

	void release(std::size_t n, const_pointer p) {
		assert(p.unique());
	}
};

} // namespace perm_group

#endif /* PERM_GROUP_ALLOCATOR_DEFAULT_HPP */