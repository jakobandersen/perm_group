#ifndef PERM_GROUP_ORBIT_HPP
#define PERM_GROUP_ORBIT_HPP

#include <perm_group/permutation/permutation.hpp>

#include <boost/dynamic_bitset.hpp>

#include <cassert>
#include <iterator>

namespace perm_group {

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

	void add(ValueType img, std::size_t positio) {
		assert(!in_orbit[img]);
		in_orbit.set(img);
	}

	// Not supported:
	// std::size_t position(ValueType u)
private:
	boost::dynamic_bitset<> in_orbit;
};

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
		assert(orbit_position[img] == 0);
		orbit_position[img] = position;
	}

	std::size_t position(ValueType u) const {
		assert(orbit_position[u] != 0);
		return orbit_position[u] - 1;
	}
private:
	// +1, 0 is used for "not in orbit"
	std::vector<ValueType> orbit_position;
};

template<typename ValueType, template<typename /*ValueType*/> class InOrbitHandler = InOrbitHandlerBitset>
struct Orbit {

	Orbit(ValueType n, ValueType w) : inOrbit(n) {
		clear(w);
	}

	void clear(ValueType w) {
		inOrbit.clear(w, *this);
		orbit.resize(1);
		orbit[0] = w;
	}

	// Given a partitioned ranged of generators with the old being from iter to iterEndOld,
	// and the new ones being from iterEndOld to iterEndNew.

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

	auto begin() const {
		return orbit.begin();
	}

	auto end() const {
		return orbit.end();
	}

	std::size_t position(ValueType u) const {
		assert(inOrbit(u));
		return inOrbit.position(u);
	}
private:
	std::vector<ValueType> orbit;
	InOrbitHandler<ValueType> inOrbit;
};

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
	orbit(w, first, last, size(**first), onNewElement, onDupElement);
}

template<typename GenPtrIter, typename OnNewElement>
void orbit(std::size_t w, const GenPtrIter &first, const GenPtrIter &last, OnNewElement onNewElement) {
	orbit(w, first, last, size(**first), onNewElement, [](auto&&...) {
	});
}

template<typename Group, typename Callback>
void orbit(std::size_t w, const Group &g, Callback callback) {
	using std::begin;
	using std::end;
	auto gens = generator_ptrs(g);
	orbit(w, begin(gens), end(gens), callback);
}

template<typename OutIter>
struct orbit_callback_output_iterator {

	orbit_callback_output_iterator(const OutIter &iter) : iter(iter) { }

	template<typename ValueType, typename PermPtrIter>
	void operator()(ValueType u, ValueType u_img, const PermPtrIter&) {
		*iter = u_img;
		++iter;
	}
public:
	OutIter iter;
};

template<typename OutIter>
orbit_callback_output_iterator<OutIter> make_orbit_callback_output_iterator(const OutIter &iter) {
	return orbit_callback_output_iterator<OutIter>(iter);
}

} // namespace perm_group

#endif /* PERM_GROUP_ORBIT_HPP */