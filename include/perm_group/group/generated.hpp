#ifndef PERM_GROUP_GROUP_GENERATED_HPP
#define PERM_GROUP_GROUP_GENERATED_HPP

#include <perm_group/allocator/allocator.hpp>
#include <perm_group/group/group.hpp>
#include <perm_group/util/iterators.hpp>

#include <cassert>
#include <vector>

namespace perm_group {

// rst: .. class:: template<typename Alloc, typename DupChecker = DupCheckerNop> \
// rst:            generated_group
// rst:
// rst:		Models `Group`.
// rst:
// rst:		Requires `Allocator<Alloc>`.
// rst:
// rst:		A basic group implementation representing it by a generating set of permutations.
// rst:
// rst:		:tparam Alloc: the allocator to use.
// rst:		:tparam DupChecker: a function object for checking if a permutation is already in the generating set.
// rst:

template<typename Alloc, typename DupChecker = DupCheckerNop>
struct generated_group {
	BOOST_CONCEPT_ASSERT((Allocator<Alloc>));
public: // Group
	// rst:		.. type:: allocator = Alloc
	using allocator = Alloc;
	// rst:		.. type:: perm = typename allocator::perm
	using perm = typename allocator::perm;
	// rst:		.. type:: pointer = typename allocator::pointer
	using pointer = typename allocator::pointer;
	// rst:		.. type:: const_pointer = typename allocator::const_pointer
	using const_pointer = typename allocator::const_pointer;
public:
	using Store = std::vector<const_pointer>;
public:

	// rst:		.. function:: explicit generated_group(const allocator &alloc, DupChecker dupChecker = DupChecker())
	// rst:
	// rst:			Construct the trivial group on `alloc.degree()` elements.

	explicit generated_group(const allocator &alloc, DupChecker dupChecker = DupChecker()) : alloc(alloc), dupChecker(dupChecker) {
		gen_set.push_back(this->alloc.make_identity());
	}

	// rst:		.. function:: explicit generated_group(std::size_t n)
	// rst:
	// rst:			Construct the trivial group on `n` elements.
	// rst:
	// rst:			Requires valid expression: `allocator(n)`

	explicit generated_group(std::size_t n) : generated_group(Alloc(n)) { }

	~generated_group() {
		for(std::size_t i = 0; i != gen_set.size(); ++i)
			alloc.release(gen_set[i]);
	}

	// rst:		.. function:: template<typename UPerm> \
	// rst:		              void add_generator(UPerm &&perm)
	// rst:
	// rst:			Add a copy of `perm` as a new generator to the group.
	// rst:			The `DupChecker` is used for deciding if the new generator already exists.
	// rst:

	template<typename UPerm>
	void add_generator(UPerm &&perm) {
		add_generator(std::forward<UPerm>(perm), [](auto&&... args) {
		});
	}

	// rst:		.. function:: template<typename UPerm, typename Next> \
	// rst:		              void add_generator(UPerm &&perm, Next next)
	// rst:
	// rst:			Add a copy of `perm` as a new generator to the group.
	// rst:			The `DupChecker` is used for deciding if the new generator already exists.
	// rst:			If the permutation is added, the `next` function is called with three iterators:
	// rst:			`next(first, lastOld, lastNew)` where the range `first` to `lastOld` denotes
	// rst:			the generating set of the group before the call, and the range `lastOld`
	// rst:			to `lastNew` denotes the added generators.
	// rst:

	template<typename UPerm, typename Next>
	void add_generator(UPerm &&perm, Next next) {
		assert(!gen_set.empty());
		auto ptr = alloc.copy(std::forward<UPerm>(perm));
		const auto gens = generators();
		const bool isDup = dupChecker(gens.begin(), gens.end(), *ptr);
		if(isDup) {
			alloc.release(ptr);
		} else {
			gen_set.push_back(ptr);
			next(gen_set.begin(), gen_set.end() - 1, gen_set.end());
		}
	}
public: // GroupConcept

	// rst:		.. function:: std::size_t degree() const

	std::size_t degree() const {
		return alloc.degree();
	}

	// rst:		.. function:: PtrContainerToPermProxy<Store, perm> generators() const

	PtrContainerToPermProxy<Store, perm> generators() const {
		return PtrContainerToPermProxy<Store, perm>(&gen_set);
	}

	// rst:		.. function:: PtrContainerToPtrProxy<Store, const_pointer> generator_ptrs() const

	PtrContainerToPtrProxy<Store, const_pointer> generator_ptrs() const {
		return PtrContainerToPtrProxy<Store, const_pointer>(&gen_set);
	}

	// rst:		.. function:: allocator &get_allocator() const

	allocator &get_allocator() const {
		return alloc;
	}
private:
	mutable allocator alloc;
	Store gen_set;
	DupChecker dupChecker;
};

} // namespace perm_group

#endif /* PERM_GROUP_GROUP_GENERATED_HPP */