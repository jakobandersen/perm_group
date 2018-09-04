#ifndef PERM_GROUP_ORBIT_HPP
#define PERM_GROUP_ORBIT_HPP

#include <perm_group/permutation/permutation.hpp>

#include <boost/dynamic_bitset.hpp>

#include <cassert>
#include <iterator>

namespace perm_group {

// rst: .. concept:: template<template<typename> class T> \
// rst:	             InOrbitHandler
// rst:
// rst:		.. notation::
// rst: 
// rst:		.. type:: value_type
// rst:
// rst:			An integer type that `T` can be instantiated with.
// rst:
// rst:		.. var:: value_type n
// rst:		         value_type i
// rst:		         value_type j
// rst:
// rst:		.. var:: T<value_type> handler
// rst:
// rst:		.. var:: Orbit orbit
// rst:
// rst:			A range of `value_types`.
// rst:
// rst:		.. valid_expr::
// rst:
// rst:		- `T<value_type>(i)`, constructible from a maximum element size `n`. 
// rst:		- `handler.clear(i, orbit)`, reset the orbit to just `i`.
// rst:		- `handler(i)`, check if `i` is in the orbit. Must return a boolean.
// rst:		- `handler.add(i, j)`, add `i` as the `j` th orbit element starting from 1. `i` is the 0th element.

template<template<typename> class T>
struct InOrbitHandler {

	BOOST_CONCEPT_USAGE(InOrbitHandler) {
		T<int> handler(42);
		const std::vector<int> orbit;
		handler.clear(42, orbit);
		res = handler(42);
		handler.add(42, 42);
	}
private:
	bool res;
};

// rst: .. class:: template<typename ValueType> \
// rst:            InOrbitHandlerBitset
// rst:
// rst:		Models `InOrbitHandler`.

template<typename ValueType>
struct InOrbitHandlerBitset {

	InOrbitHandlerBitset(ValueType n) : in_orbit(n) { }

	template<typename Orbit>
	void clear(ValueType w, const Orbit &orbit) {
		// we assume each orbit is sparse
		for(auto o : orbit)
			in_orbit.reset(o);
		in_orbit.set(w);
	}

	bool operator()(ValueType u) const {
		return in_orbit[u];
	}

	void add(ValueType img, std::size_t position) {
		assert(!in_orbit[img]);
		in_orbit.set(img);
	}

	// Not supported:
	// std::size_t position(ValueType u)
private:
	boost::dynamic_bitset<> in_orbit;
};


// rst: .. class:: template<typename ValueType> \
// rst:            InOrbitHandlerVector
// rst:
// rst:		Models `InOrbitHandler`.

template<typename ValueType>
struct InOrbitHandlerVector {

	InOrbitHandlerVector(ValueType n) : orbit_position(n, 0) { }

	template<typename Orbit>
	void clear(ValueType w, const Orbit &orbit) {
		// we assume each orbit is sparse
		for(auto o : orbit)
			orbit_position[o] = 0;
		orbit_position[w] = 1;
	}

	bool operator()(ValueType u) const {
		return orbit_position[u] != 0;
	}

	void add(ValueType img, std::size_t position) {
		orbit_position[img] = position;
	}

	// rst:		.. function:: ValueType position(ValueType u) const
	// rst:
	// rst:			Requires the clear method to have been called.
	// rst:			Requires either `u` has been added or was the element given to the latest clear.
	// rst:
	// rst:			:returns: the position of `u` in the orbit. The initial orbit element is at position 0.

	ValueType position(ValueType u) const {
		return orbit_position[u] - 1;
	}
private:
	// +1, 0 is used for "not in orbit"
	std::vector<ValueType> orbit_position;
};

// rst: .. class:: template<typename ValueType, template<typename> class InOrbitHandlerT = InOrbitHandlerBitset> \
// rst:            Orbit
// rst:
// rst:		Requires `InOrbitHandler<InOrbitHandlerT>`.
// rst:
// rst:		An orbit calculator, which also acts as a range of the calculated orbit elements.
// rst:

template<typename ValueType, template<typename /*ValueType*/> class InOrbitHandlerT = InOrbitHandlerBitset>
struct Orbit {
	BOOST_CONCEPT_ASSERT((InOrbitHandler<InOrbitHandlerT>));

	// rst:		.. function:: Orbit(ValueType n, ValueType w)
	// rst:
	// rst:			Construct with a maximum element size `n` and an initial orbit element `w`.

	Orbit(ValueType n, ValueType w) : inOrbit(n) {
		clear(w);
	}

	// rst:		.. function:: void clear(ValueType w)
	// rst:
	// rst:			Reset the object to an orbit with just `w`.

	void clear(ValueType w) {
		inOrbit.clear(w, *this);
		orbit.resize(1);
		orbit[0] = w;
	}

	// rst:		.. function:: template<typename GenPtrIter, typename OnNewElement, typename OnDupElement> \
	// rst:		              bool update(const GenPtrIter &first, const GenPtrIter &lastOld, const GenPtrIter &lastNew, OnNewElement onNewElement, OnDupElement onDupElement)
	// rst:
	// rst:			Requires the iterators to iterator over pointer-like values to permutations (`Permutation`).
	// rst:			That is, `Permutation<typename std::pointer_traits<typename std::iterator_traits<GenPtrIter>::value_type>::element_type>`.
	// rst:
	// rst:			Given two ranges of generators, `first` to `lastOld` and `lastOld` to `lastNew`,
	// rst:			extend the current orbit using first the new generators, `lastOld` to `lastNew`, and then with all generators.
	// rst:
	// rst:			When a new orbit element `o` is found, by mapping a known orbit element `w` through a permutation pointed to by an iterator `it`,
	// rst:			then `onNewElement(w, o, it)` is called.
	// rst:			Similarly, when `o` is already in the orbit then `onDupElement(w, o, it)` is called.

	template<typename GenPtrIter, typename OnNewElement, typename OnDupElement>
	bool update(const GenPtrIter &first, const GenPtrIter &lastOld, const GenPtrIter &lastNew, OnNewElement onNewElement, OnDupElement onDupElement) {
		// typedefs to trigger erroneous types
		using PermPtr = typename std::iterator_traits<GenPtrIter>::value_type;
		using Perm = typename std::pointer_traits<PermPtr>::element_type;

		const std::size_t prevEnd = orbit.size();
		// run the new ones on the old points
		for(GenPtrIter it = lastOld; it != lastNew; ++it) { // this might be empty, so have that as outer
			for(std::size_t i = 0; i != prevEnd; ++i) {
				const auto oi = orbit[i];
				const auto img = perm_group::get(**it, oi);
				if(!inOrbit(img)) {
					orbit.push_back(img);
					inOrbit.add(img, orbit.size());
					onNewElement(oi, img, it);
				} else {
					onDupElement(oi, img, it);
				}
			}
		}
		// now run all of them on all the new points
		for(std::size_t i = prevEnd; i != orbit.size(); ++i) { // the end might change
			const auto oi = orbit[i];
			for(GenPtrIter it = first; it != lastNew; ++it) {
				const auto img = perm_group::get(**it, oi);
				if(!inOrbit(img)) {
					orbit.push_back(img);
					inOrbit.add(img, orbit.size());
					onNewElement(oi, img, it);
				} else {
					onDupElement(oi, img, it);
				}
			}
		}
		return prevEnd == orbit.size();
	}

	// rst:		.. function:: auto begin() const
	// rst:
	// rst:			:returns: an iterator to the first orbit element.

	auto begin() const {
		return orbit.begin();
	}

	// rst:		.. function:: auto end() const
	// rst:
	// rst:			:returns: an iterator to past-the-end of the orbit.

	auto end() const {
		return orbit.end();
	}

	// rst:		.. todo:: document

	bool isInOrbit(ValueType u) const {
		return inOrbit(u);
	}


	// rst:		.. function:: ValueType position(ValueType u) const
	// rst:
	// rst:			Requires `InOrbitHandlerT` to have a position method.
	// rst:
	// rst:			Returns the position of `u` in the orbit (starting from 0).
	// rst:			See also `InOrbitHandlerVector::position`.

	ValueType position(ValueType u) const {
		return inOrbit.position(u);
	}
private:
	std::vector<ValueType> orbit;
	InOrbitHandlerT<ValueType> inOrbit;
};

// rst: .. function:: template<typename GenPtrIter, typename OnNewElement, typename OnDupElement> \
// rst:               void orbit(std::size_t w, const GenPtrIter &first, const GenPtrIter &last, std::size_t n, OnNewElement onNewElement, OnDupElement onDupElement)
// rst:               template<typename GenPtrIter, typename OnNewElement, typename OnDupElement> \
// rst:               void orbit(std::size_t w, const GenPtrIter &first, const GenPtrIter &last, OnNewElement onNewElement, OnDupElement onDupElement)
// rst:               template<typename GenPtrIter, typename OnNewElement> \
// rst:               void orbit(std::size_t w, const GenPtrIter &first, const GenPtrIter &last, OnNewElement onNewElement)
// rst:
// rst:		Calculate the orbit of `w` under the non-empty range of generators `first` to `last`.
// rst:		See `Orbit::update` for the details of `first`, `last`, `onNewElement`, and `onDupElement`.
// rst:		Note though that `onNewElement(w, w, last)` is called in the beginning in addition.
// rst:		The overload without `n` requires `SizeAwarePermutation<decltype(**first)>` and sets `n = perm_group::degree(**first)`.

template<typename GenPtrIter, typename OnNewElement, typename OnDupElement>
void orbit(std::size_t w, const GenPtrIter &first, const GenPtrIter &last, std::size_t n, OnNewElement onNewElement, OnDupElement onDupElement) {
	using PermPtr = typename std::iterator_traits<GenPtrIter>::value_type;
	using Perm = typename std::pointer_traits<PermPtr>::element_type;
	using value_type = typename permutation_traits<Perm>::value_type;
	Orbit<value_type> orbit(n, w);
	onNewElement(w, w, last);
	orbit.update(first, first, last, onNewElement, onDupElement);
}

template<typename GenPtrIter, typename OnNewElement, typename OnDupElement>
void orbit(std::size_t w, const GenPtrIter &first, const GenPtrIter &last, OnNewElement onNewElement, OnDupElement onDupElement) {
	orbit(w, first, last, perm_group::degree(**first), onNewElement, onDupElement);
}

template<typename GenPtrIter, typename OnNewElement>
void orbit(std::size_t w, const GenPtrIter &first, const GenPtrIter &last, OnNewElement onNewElement) {
	orbit(w, first, last, perm_group::degree(**first), onNewElement, [](auto&&... args) {
	});
}

// rst: .. function:: template<typename Group, typename Callback> \
// rst:               void orbit(std::size_t w, const Group &g, Callback callback)
// rst:
// rst:		Calculate the orbit of `w` in the group `g`. See `Orbit::update::onNewElement` for the meaning of `callback`.

template<typename Group, typename Callback>
void orbit(std::size_t w, const Group &g, Callback callback) {
	using std::begin;
	using std::end;
	auto gens = g.generator_ptrs();
	orbit(w, begin(gens), end(gens), callback);
}

template<typename OutIter>
struct orbit_callback_output_iterator {

	orbit_callback_output_iterator(OutIter iter) : iter(iter) { }

	template<typename ValueType, typename PermPtrIter>
	void operator()(ValueType u, ValueType u_img, const PermPtrIter&) {
		*iter = u_img;
		++iter;
	}
public:
	OutIter iter;
};

// rst: .. function:: template<typename OutIter> \
// rst:               orbit_callback_output_iterator<OutIter> make_orbit_callback_output_iterator(OutIter iter)
// rst:
// rst:		:returns: a callback usable in the `Orbit::update` or `orbit` functions that outputs the elements to the given output iterator `iter`.

template<typename OutIter>
orbit_callback_output_iterator<OutIter> make_orbit_callback_output_iterator(OutIter iter) {
	return orbit_callback_output_iterator<OutIter>(iter);
}

} // namespace perm_group

#endif /* PERM_GROUP_ORBIT_HPP */