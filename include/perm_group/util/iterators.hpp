#ifndef PERM_GROUP_UTIL_ITERATORS_HPP
#define PERM_GROUP_UTIL_ITERATORS_HPP

#include <boost/iterator/transform_iterator.hpp>

namespace perm_group {

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

	using iterator = boost::transform_iterator<iterator_transformer, typename Container::const_iterator>;
public:

	PtrContainerToPermProxy(const Container *g) : g(g) { }

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

	using iterator = boost::transform_iterator<iterator_transformer, typename Container::const_iterator>;
public:

	PtrContainerToPtrProxy(const Container *g) : g(g) { }

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