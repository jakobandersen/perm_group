#ifndef PERM_GROUP_UTIL_ITERATORS_HPP
#define PERM_GROUP_UTIL_ITERATORS_HPP

#include <boost/iterator/transform_iterator.hpp>

namespace perm_group {

// rst: This file contains convenience class templates for implementtors
// rst: for constructing read-only ranges of permutations.
// rst: 

// rst: .. class:: template<typename Container, typename Perm>\
// rst:            PtrContainerToPermProxy
// rst:
// rst:		A view of a given container of pointers to permutations,
// rst:		exposing it as a range of constant permutations.
// rst:

template<typename Container, typename Perm>
struct PtrContainerToPermProxy {
	using Pointer = typename Container::value_type;

	struct iterator_transformer {

		iterator_transformer() : g(nullptr) { }

		iterator_transformer(const Container *g) : g(g) { }

		const Perm &operator()(Pointer p) const {
			return *p;
		}
	private:
		const Container *g;
	};

	// rst:		.. type:: iterator
	// rst:
	// rst:			An adaptor of `Container::const_iterator`.
	// rst:			It has `const Perm&` as `iterator::value_type`.
	using iterator = boost::transform_iterator<iterator_transformer, typename Container::const_iterator>;
public:

	// rst:		.. function:: PtrContainerToPermProxy(const Container *g)

	PtrContainerToPermProxy(const Container *g) : g(g) { }

	// rst:		.. function:: typename Container::size_type size() const
	// rst:		              iterator begin() const
	// rst:		              iterator end() const
	// rst:		              const Perm &operator[](std::size_t i) const
	// rst:
	// rst:			Calls the corresponding methods on the underlying container.

	typename Container::size_type size() const {
		return g->size();
	}

	iterator begin() const {
		return iterator(g->begin(), iterator_transformer(g));
	}

	iterator end() const {
		return iterator(g->end(), iterator_transformer(g));
	}

	const Perm &operator[](std::size_t i) const {
		assert(i < size());
		return begin()[i];
	}
private:
	const Container *g;
};

// rst:
// rst: .. class:: template<typename Container, typename Perm>\
// rst:            PtrContainerToPtrProxy
// rst:
// rst:		A view of a given container of pointers to permutations,
// rst:		exposing it as a range of `ConstPointer`.
// rst:

template<typename Container, typename ConstPointer>
struct PtrContainerToPtrProxy {
	using Pointer = typename Container::value_type;

	struct iterator_transformer {

		iterator_transformer(const Container *g) : g(g) { }

		ConstPointer operator()(Pointer p) const {
			return p;
		}
	private:
		const Container *g;
	};

	// rst:		.. type:: iterator
	// rst:
	// rst:			An adaptor of `Container::const_iterator`.
	// rst:			It has `ConstPointer` as `iterator::value_type`.
	using iterator = boost::transform_iterator<iterator_transformer, typename Container::const_iterator>;
public:

	// rst:		.. function:: PtrContainerToPtrProxy(const Container *g)

	PtrContainerToPtrProxy(const Container *g) : g(g) { }

	// rst:		.. function:: typename Container::size_type size() const
	// rst:		              iterator begin() const
	// rst:		              iterator end() const
	// rst:		              const Perm &operator[](std::size_t i) const
	// rst:
	// rst:			Calls the corresponding methods on the underlying container.

	typename Container::size_type size() const {
		return g->size();
	}

	iterator begin() const {
		return iterator(g->begin(), iterator_transformer(g));
	}

	iterator end() const {
		return iterator(g->end(), iterator_transformer(g));
	}

	ConstPointer operator[](std::size_t i) const {
		assert(i < size());
		return begin()[i];
	}
private:
	const Container *g;
};

} // namespace perm_group

#endif /* PERM_GROUP_UTIL_ITERATORS_HPP */