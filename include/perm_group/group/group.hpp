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
// rst:		.. type:: allocator = typename GroupT::allocator
// rst:		.. type:: perm = typename GroupT::perm
// rst:		.. type:: pointer = typename allocator::pointer
// rst:		.. type:: const_pointer = typename allocator::const_pointer
// rst:
// rst:		Requires `Permutation<perm>` and `Allocator<allocator>`.
// rst:

template<typename GroupT>
struct Group {
	using allocator = typename GroupT::allocator;
	using perm = typename GroupT::perm;
	using pointer = typename allocator::pointer;
	using const_pointer = typename allocator::const_pointer;

	BOOST_CONCEPT_ASSERT((Permutation<perm>));
	BOOST_CONCEPT_ASSERT((Allocator<allocator>));

	// rst:		.. notation::
	// rst:
	// rst:		.. var:: GroupT g
	// rst:		.. var:: const GroupT cg
	// rst:
	// rst:		.. valid_expr::
	// rst:
	// rst:		- `cg.degree()`, returns an integer with the number of elements the group is defined on.
	// rst:		- `cg.generators()`, returns a non-empty random-access range of `perm`.
	// rst:		- `cg.generator_ptrs()`, returns a random-access range of pointer-like values to `perm`.
	// rst:		  The range and order of the dereferenced pointers must be exactly the range returned by `cg.generators()`.
	// rst:		- `cg.get_allocator()`, returns the allocator used by the group.

	BOOST_CONCEPT_USAGE(Group) {
		using std::begin;
		using std::end;
		std::size_t n = cGroup->degree();
		(void) n;
		{
			auto gs = cGroup->generators();
			using iterator = typename decltype(gs)::iterator;
			iterator first = begin(gs);
			iterator last = end(gs);
			const perm &p = *first;
			(void) last;
			(void) p;
		}
		{
			auto gs = cGroup->generator_ptrs();
			using iterator = typename decltype(gs)::iterator;
			iterator first = begin(gs);
			iterator last = end(gs);
			const_pointer ptr = *first;
			(void) last;
			(void) ptr;
		}
		allocator alloc = cGroup->get_allocator();
		(void) alloc;
	}
private:
	const GroupT *cGroup;
};

// rst:
// rst: .. concept:: template<typename GroupT> \
// rst:              Stabilizer
// rst:
// rst:		A refinement of the `Group` concept.
// rst:
// rst:		A stabilizer fixes a specific element in some base group.
// rst:		The user must keep track of this base group and what information has been given to the stabilizer.
// rst:
// rst:		Requires `Moveable<GroupT>`.
// rst:

template<typename GroupT>
struct Stabilizer : Group<GroupT> {
	// from Group
	using allocator = typename GroupT::allocator;
	using perm = typename GroupT::perm;
	using pointer = typename allocator::pointer;
	using const_pointer = typename allocator::const_pointer;
public:
	// rst:		.. assoc_types::
	// rst:
	// rst:		.. type:: is_accurate = typename GroupT::is_accurate
	// rst:
	// rst:			A type convertible to `std::true_type` or `std::false_type`,
	// rst:			denoting whether the group will represent the full stabilizer
	// rst:			or if it potentially only represents a proper subgroup of the full stabilizer.
	// rst:
	using is_accurate = typename GroupT::is_accurate;
	// rst:		.. notation::
	// rst:
	// rst:		.. var:: GroupT g
	// rst:		.. var:: const GroupT cg
	// rst:		.. var:: PtrIterator first
	// rst:		         PtrIterator lastOld
	// rst:		         PtrIterator lastNew
	// rst:
	// rst:			Iterators over `GroupT::const_pointer` denoting the consequtive
	// rst:			ranges `first` to `lastOld` and `lastOld` to `lastNew`.
	// rst:
	// rst:		.. var:: Callback callback
	// rst:
	// rst:			A `Callable` accepting three iterator type of equal type.
	// rst:			The iterators are the type of pointer iterators given by this group in its `cg.generator_ptrs()` method.
	// rst:
	// rst:		.. valid_expr::
	// rst:
	// rst:		- `cg.fixed_element()`, returning an element which defines this stablizer.
	// rst:		- `g.add_generators(first, lastOld, lastNew)`: notifies this stabilizer that the base group
	// rst:		  has been extended with the permutations denoted indirectly by `lastOld` to `lastNew`.
	// rst:		  The range `first` to `lastOld` must be the range previously given to this stabilizer as `first` to `lastNew`.
	// rst:		  If not previous `g.add_generators` call has been made, then `first == lastNew`.
	// rst:
	// rst:		  The stabilizer will decide if these new permutations in the base group warrents an extension
	// rst:		  of its generating set. This decision must be in accordance with the `is_accurate` type.
	// rst:		- `g.add_generators(first, lastOld, lastNew, callback)`: as the other `g.add_generators` function,
	// rst:		  but with a notification callback. If new permutations are added to this stabilizer,
	// rst:		  the callback will be called at least once with three pointer iterators `stabFirst`, `stabLastOld`, and `stabLastNew`.
	// rst:		  These iterators have the same semantics as the given iterators, but for the stabilizer it self.

	BOOST_CONCEPT_USAGE(Stabilizer) {
		// the element that is fixed in the stabilizer
		std::size_t fixed = cGroup->fixed_element();
		(void) fixed;
		// Tell the stabilizer about a permutation in the parent group.
		// It will then potentially add permutations to it self.
		using PointerIter = const_pointer*;
		group.add_generators(PointerIter(), PointerIter(), PointerIter());
		group.add_generators(PointerIter(), PointerIter(), PointerIter(), [](auto, auto, auto) {
		});
	}
private:
	GroupT group;
	const GroupT *cGroup;
};

struct DupCheckerNop {

	template<typename Iter>
	bool operator()(Iter first, Iter last, const typename std::iterator_traits<Iter>::value_type &perm) {
		return false;
	}
};

struct DupCheckerCompare {

	template<typename Iter>
	bool operator()(Iter first, Iter last, const typename std::iterator_traits<Iter>::value_type &perm) {
		for(; first != last; ++first)
			if(*first == perm)
				return true;
		return false;
	}
};

} // namespace perm_group

#endif /* PERM_GROUP_GROUP_GROUP_HPP */