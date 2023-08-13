#ifndef PERM_GROUP_GROUP_BASIC_STABILIZER_HPP
#define PERM_GROUP_GROUP_BASIC_STABILIZER_HPP

#include <perm_group/allocator/allocator.hpp>
#include <perm_group/group/group.hpp>
#include <perm_group/permutation/traits.hpp>
#include <perm_group/util/iterators.hpp>

#include <cassert>
#include <vector>

namespace perm_group {

//------------------------------------------------------------------------------
// Basic Stabilizer
//------------------------------------------------------------------------------

// rst: .. class:: template<typename Alloc, typename DupChecker = DupCheckerNop> basic_stabilizer
// rst:
// rst:		A  `Stabilizer` type which filters permutations from a base group
// rst:		simply by whether they fix a given element.
// rst:		It may therefore represent only a subgroup of the stabilizer it is
// rst:		defined as.
// rst:

template<typename Alloc, typename DupChecker = DupCheckerNop>
struct basic_stabilizer {
	BOOST_CONCEPT_ASSERT((Allocator<Alloc>));
public: // Group
	// rst:		.. type:: allocator = Alloc
	using allocator = Alloc;
	// rst:		.. type::  perm = typename allocator::perm
	using perm = typename allocator::perm;
	// rst:		.. type:: pointer = typename allocator::pointer
	using pointer = typename allocator::pointer;
	// rst:		.. type:: const_pointer = typename allocator::const_pointer
	using const_pointer = typename allocator::const_pointer;
public: // Stabilizer
	// rst:		.. type:: is_accurate = std::false_type
	using is_accurate = std::false_type;
public:
	using Store = std::vector<const_pointer>;
public:

	// rst:		.. function:: basic_stabilizer(std::size_t fixed, const allocator &alloc, DupChecker dupChecker = DupChecker())

	basic_stabilizer(std::size_t fixed, const allocator &alloc, DupChecker dupChecker = DupChecker())
	: fixed(fixed), alloc(alloc), dupChecker(dupChecker) {
		gen_set.push_back(this->alloc.make_identity());
	}

	basic_stabilizer(basic_stabilizer &&other) : basic_stabilizer(other.fixed, other.alloc, other.dupChecker) {
		// steal everything they got, but give them our newly created id permutation
		using std::swap;
		swap(gen_set, other.gen_set);
	}

	basic_stabilizer &operator=(basic_stabilizer &&other) {
		using std::swap;
		swap(fixed, other.fixed);
		swap(alloc, other.alloc);
		swap(gen_set, other.gen_set);
		swap(dupChecker, other.dupChecker);
		return *this;
	}

	basic_stabilizer(const basic_stabilizer&) = delete;
	basic_stabilizer &operator=(const basic_stabilizer&) = delete;

	~basic_stabilizer() {
		alloc.release(gen_set.front());
	}
public: // GroupConcept

	// rst:		.. function:: std::size_t degree() const

	std::size_t degree() const {
		return alloc.degree();
	}

	// rst:		.. function:: PtrContainerToPermProxy<Store, perm> generators() const

	PtrContainerToPermProxy<Store, perm> generators() const {
		return PtrContainerToPermProxy<Store, perm>(gen_set);
	}

	// rst:		.. function:: PtrContainerToPtrProxy<Store, const_pointer> generator_ptrs() const

	PtrContainerToPtrProxy<Store, const_pointer> generator_ptrs() const {
		return PtrContainerToPtrProxy<Store, const_pointer>(gen_set);
	}

	// rst:		.. function:: const allocator &get_allocator() const

	const allocator &get_allocator() const {
		return alloc;
	}
public: // StabilizerConcept

	// rst:		.. function:: std::size_t fixed_element() const

	std::size_t fixed_element() const {
		return fixed;
	}

	// rst:		.. function:: template<typename Iter> \
	// rst:		              void add_generators(Iter first, Iter lastOld, Iter lastNew)

	template<typename Iter>
	void add_generators(Iter first, Iter lastOld, Iter lastNew) {
		add_generators(first, lastOld, lastNew, [](auto&&... args) {
		});
	}

	// rst:		.. function:: template<typename Iter, typename Next> \
	// rst:		              void add_generators(Iter first, Iter lastOld, Iter lastNew, Next next)

	template<typename Iter, typename Next>
	void add_generators(Iter first, Iter lastOld, Iter lastNew, Next next) {
		const auto oldSize = gen_set.size();
		for(; lastOld != lastNew; ++lastOld) {
			if(perm_group::get(**lastOld, fixed) == fixed) {
				const auto gens = generators();
				const bool isDup = dupChecker(gens.begin(), gens.end(), **lastOld);
				if(!isDup)
					gen_set.push_back(*lastOld);
			}
		}
		if(gen_set.size() != oldSize)
			next(gen_set.begin(), gen_set.begin() + oldSize, gen_set.end());
	}
protected:
	std::size_t fixed;
	mutable allocator alloc;
	Store gen_set;
	DupChecker dupChecker;
};

} // namespace perm_group

#endif /* PERM_GROUP_GROUP_BASIC_STABILIZER_HPP */