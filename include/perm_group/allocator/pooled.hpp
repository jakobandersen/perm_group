#ifndef PERM_GROUP_ALLOCATOR_POOLED_HPP
#define PERM_GROUP_ALLOCATOR_POOLED_HPP

#include <perm_group/allocator/allocator.hpp>
#include <perm_group/permutation/permutation.hpp>

#include <boost/concept_check.hpp>

#include <memory>
#include <type_traits>
#include <vector>

namespace perm_group {

// rst: .. class:: template<typename Alloc> pooled_allocator
// rst:
// rst:		An `Allocator` adaptor that retains a pool of unused permutations
// rst:		that has previously been released. Excess permutations are released.
// rst:		A copy of this allocator still refers to the same pool as the original.
// rst:

template<typename Alloc>
struct pooled_allocator {
	// rst:		.. type:: perm = typename Alloc::perm
	using perm = typename Alloc::perm;
	BOOST_CONCEPT_ASSERT((boost::Assignable<perm>));
	// rst:		.. type:: pointer = typename Alloc::pointer
	using pointer = typename Alloc::pointer;
	// rst:		.. type:: const_pointer = typename Alloc::const_pointer
	using const_pointer = typename Alloc::const_pointer;
public:
	using Store = std::vector<pointer>;
public:

	// rst:		.. type: explicit pooled_allocator(std::size_t pool_size, Alloc alloc)
	// rst:
	// rst:			Construct an allocator with a given maximum pool size, using the given nested allocator.

	explicit pooled_allocator(std::size_t pool_size, Alloc alloc)
	: pool_size(pool_size), alloc(alloc), pool(std::make_shared<Store>()) {
		pool->reserve(pool_size);
	}

	// rst:		.. function:: ~pooled_allocator()
	// rst:
	// rst:			If this is the last copy of this allocator, the pool is released.

	~pooled_allocator() {
		if(pool.unique()) {
			for(pointer p : *pool)
				alloc.release(p);
		}
	}

	// rst:		.. function:: std::size_t degree() const

	std::size_t degree() const {
		return alloc.degree();
	}

	// rst:		.. function:: pointer make()

	pointer make() {
		if(pool->empty()) {
			return alloc.make();
		} else {
			pointer p = pool->back();
			pool->pop_back();
			return p;
		}
	}

	// rst:		.. function:: pointer make_identity()

	pointer make_identity() {
		if(pool->empty()) {
			return alloc.make_identity();
		} else {
			pointer p = pool->back();
			pool->pop_back();
			for(std::size_t i = 0; i != degree(); ++i)
				perm_group::put(*p, i, i);
			return p;
		}
	}

	// rst:		.. function:: template<typename UPerm> pointer copy(UPerm &&p)

	template<typename UPerm>
	pointer copy(UPerm &&other) {
		if(pool->empty()) {
			return alloc.copy(std::forward<UPerm>(other));
		} else {
			pointer p = pool->back();
			pool->pop_back();
			copy_dispatch(p, other,
					std::is_same<perm,
					/**/ typename std::remove_cv<
					/*****/ typename std::remove_reference<UPerm>::type
					/**/ >::type
					> ());
			return p;
		}
	}

	// rst:		.. function:: void release(const_pointer p)

	void release(const_pointer p) {
		if(!p) return;
		if(pool->size() > pool_size)
			alloc.release(p);
		else
			pool->push_back(const_cast<pointer> (p));
	}
private:

	template<typename Other>
	void copy_dispatch(pointer p, const Other &other, std::true_type) {
		*p = other;
	}

	template<typename Other>
	void copy_dispatch(pointer p, const Other &other, std::false_type) {
		for(std::size_t i = 0; i != degree(); ++i)
			perm_group::put(*p, i, perm_group::get(other, i));
	}
private:
	std::size_t pool_size;
	Alloc alloc;
	std::shared_ptr<Store> pool;
};

} // namespace perm_group

#endif /* PERM_GROUP_ALLOCATOR_POOLED_HPP */