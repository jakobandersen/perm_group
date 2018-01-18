#ifndef PERM_GROUP_GROUP_GENERATED_HPP
#define PERM_GROUP_GROUP_GENERATED_HPP

#include <perm_group/allocator/default.hpp>
#include <perm_group/group/group.hpp>
#include <perm_group/util/iterators.hpp>

#include <cassert>
#include <vector>

namespace perm_group {

// rst: .. class:: template<typename Perm, typename Alloc = default_allocator<Perm> > \
// rst:            generated_group
// rst:
// rst:		Models `Group`.
// rst:
// rst:		Requires `Permutation<Perm>` and `Allocator<Alloc>`.
// rst:
// rst:		A basic group implementation representing it by a generating set of permutations.
// rst:
template<typename Perm, typename Alloc = default_allocator<Perm> >
struct generated_group {
public: // Group
	using perm_type = Perm;
	using allocator = Alloc;
	using pointer = typename allocator::pointer;
	using const_pointer = typename allocator::const_pointer;
public:
	using Store = std::vector<const_pointer>;
public:

	// rst:		.. function:: generated_group(std::size_t n, Alloc alloc = Alloc())
	// rst:
	// rst:			Construct the trivial group on `n` elements.
	generated_group(std::size_t n, Alloc alloc = Alloc())
	: n(n), alloc(alloc) {
		gen_set.push_back(this->alloc.make_identity(n));
	}

	~generated_group() {
		for(std::size_t i = 0; i < gen_set.size(); ++i)
			alloc.release(n, std::move(gen_set[i]));
	}

	// rst:		.. function:: template<typename UPerm> \
	// rst:		              void add_generator(UPerm &&perm)
	// rst:
	// rst:			Add a copy of `perm` as a new generator to the group.
	template<typename UPerm>
	void add_generator(UPerm &&perm) {
		assert(!gen_set.empty());
		pointer ptr = alloc.copy(n, std::forward<UPerm>(perm));
		gen_set.push_back(ptr);
	}
public: // GroupConcept

	friend std::size_t degree(const generated_group &g) {
		return g.n;
	}

	friend PtrContainerToPermProxy<Store, perm_type> generators(const generated_group &g) {
		return PtrContainerToPermProxy<Store, perm_type>(&g.gen_set);
	}

	friend PtrContainerToPtrProxy<Store, const_pointer> generator_ptrs(const generated_group &g) {
		return PtrContainerToPtrProxy<Store, const_pointer>(&g.gen_set);
	}

	friend allocator &get_allocator(const generated_group &g) {
		return g.alloc;
	}
private:
	std::size_t n;
	mutable Alloc alloc;
	Store gen_set;
};

} // namespace perm_group

#endif /* PERM_GROUP_GROUP_GENERATED_HPP */