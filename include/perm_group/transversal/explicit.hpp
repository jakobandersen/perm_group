#ifndef PERM_GROUP_TRANSVERSAL_EXPLICIT_HPP
#define PERM_GROUP_TRANSVERSAL_EXPLICIT_HPP

#include <perm_group/orbit.hpp>
#include <perm_group/permutation/mult.hpp>
#include <perm_group/permutation/traits.hpp>

namespace perm_group {

template<typename Allocator>
struct transversal_explicit {
	using allocator = Allocator;
	using perm_type = typename allocator::perm_type;
	using value_type = typename permutation_traits<perm_type>::value_type;
	using pointer = typename allocator::pointer;
	using const_pointer = typename allocator::const_pointer;
	using orbit_type = Orbit<value_type, InOrbitHandlerVector>;
public:

	transversal_explicit(std::size_t n, value_type w, Allocator alloc) : n(n), alloc(alloc), orbit(n, w) {
		perms.push_back(this->alloc.make_identity(n));
		pred.push_back(w);
	}

	~transversal_explicit() {
		for(std::size_t i = 0; i < perms.size(); ++i)
			alloc.release(n, std::move(perms[i]));
	}

	friend const orbit_type &orbit(const transversal_explicit &t) {
		return t.orbit;
	}

	template<typename GenPtrIter, typename OnNewElement, typename OnDupElement>
	bool update(const GenPtrIter &first, const GenPtrIter &lastOld, const GenPtrIter &lastNew, OnNewElement onNewElement, OnDupElement onDupElement) {
		// typedefs to trigger erroneous types
		using PermPtr = typename std::iterator_traits<GenPtrIter>::value_type;
		using Perm = typename std::pointer_traits<PermPtr>::element_type;

		const auto inner_onNewElement = [this, onNewElement](value_type u, value_type u_img, const GenPtrIter & iter_perm) {
			const auto &trans_u = from_element(u);
			const_pointer trans_u_img = alloc.copy(n, perm_group::mult(trans_u, **iter_perm));
			perms.push_back(trans_u_img);
			pred.push_back(u);
			onNewElement(u, u_img, iter_perm, trans_u_img);
		};
		const auto inner_onDupElement = [this, onDupElement](value_type u, value_type u_img, const GenPtrIter & iter_perm) {
			const_pointer p = from_element_as_ptr(u);
			onDupElement(u, u_img, iter_perm, p);
		};
		return orbit.update(first, lastOld, lastNew, inner_onNewElement, inner_onDupElement);
	}

	template<typename GenPtrIter>
	bool update(const GenPtrIter &first, const GenPtrIter &lastOld, const GenPtrIter &lastNew) {
		const auto nop = [](auto&&...) {
		};
		return update(first, lastOld, lastNew, nop, nop);
	}

	const perm_type &from_element(value_type o) const {
		return *from_element_as_ptr(o);
	}

	const_pointer from_element_as_ptr(value_type o) const {
		return perms[orbit.position(o)];
	}

	value_type predecessor(value_type o) const {
		return pred[orbit.position(o)];
	}
private:
	std::size_t n;
	Allocator alloc;
	orbit_type orbit;
	std::vector<const_pointer> perms;
	std::vector<value_type> pred;
};

} // namespace perm_group

#endif /* PERM_GROUP_TRANSVERSAL_EXPLICIT_HPP */