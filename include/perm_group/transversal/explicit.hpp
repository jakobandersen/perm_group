#ifndef PERM_GROUP_TRANSVERSAL_EXPLICIT_HPP
#define PERM_GROUP_TRANSVERSAL_EXPLICIT_HPP

#include <perm_group/orbit.hpp>
#include <perm_group/permutation/mult.hpp>
#include <perm_group/permutation/traits.hpp>

namespace perm_group {

// rst: .. todo:: Actually write the documentation for this file.

template<typename Alloc>
struct transversal_explicit {
public: // Transversal requirements
	using allocator = Alloc;
public:
	using perm = typename allocator::perm;
	using value_type = typename permutation_traits<perm>::value_type;
	using pointer = typename allocator::pointer;
	using const_pointer = typename allocator::const_pointer;
public: // More Transversal requirements
	using orbit_type = Orbit<value_type, InOrbitHandlerVector>;
public:

	transversal_explicit(value_type w, const allocator &alloc) : alloc(alloc), orbit_(alloc.degree(), w) {
		perms.push_back(this->alloc.make_identity());
		pred.push_back(w);
	}

	transversal_explicit(transversal_explicit &&other) : transversal_explicit(other.get_root(), other.get_allocator()) {
		using std::swap;
		swap(orbit_, other.orbit_);
		swap(perms, other.perms);
		swap(pred, other.pred);
	}

	transversal_explicit &operator=(transversal_explicit &&other) {
		using std::swap;
		swap(alloc, other.alloc);
		swap(orbit_, other.orbit_);
		swap(perms, other.perms);
		swap(pred, other.pred);
		return *this;
	}

	transversal_explicit(const transversal_explicit&) = delete;
	transversal_explicit &operator=(const transversal_explicit&) = delete;

	~transversal_explicit() {
		for(std::size_t i = 0; i != perms.size(); ++i)
			alloc.release(std::move(perms[i]));
	}
public:

	allocator &get_allocator() const {
		return alloc;
	}

	const orbit_type &orbit() const {
		return orbit_;
	}

	template<typename GenPtrIter, typename OnNewElement, typename OnDupElement>
	bool update(const GenPtrIter &first, const GenPtrIter &lastOld, const GenPtrIter &lastNew, OnNewElement onNewElement, OnDupElement onDupElement) {
		// typedefs to trigger erroneous types
		using PermPtr = typename std::iterator_traits<GenPtrIter>::value_type;
		using Perm = typename std::pointer_traits<PermPtr>::element_type;

		const auto inner_onNewElement = [this, onNewElement](value_type u, value_type u_img, const GenPtrIter & iter_perm) {
			const auto &trans_u = from_element(u);
			const_pointer trans_u_img = alloc.copy(perm_group::mult(trans_u, **iter_perm));
			perms.push_back(trans_u_img);
			pred.push_back(u);
			onNewElement(u, u_img, iter_perm, trans_u_img);
		};
		const auto inner_onDupElement = [this, onDupElement](value_type u, value_type u_img, const GenPtrIter & iter_perm) {
			const_pointer p = from_element_as_ptr(u);
			onDupElement(u, u_img, iter_perm, p);
		};
		return orbit_.update(first, lastOld, lastNew, inner_onNewElement, inner_onDupElement);
	}

	template<typename GenPtrIter>
	bool update(const GenPtrIter &first, const GenPtrIter &lastOld, const GenPtrIter &lastNew) {
		const auto nop = [](auto&&... args) {
		};
		return update(first, lastOld, lastNew, nop, nop);
	}

	const perm &from_element(value_type o) const {
		return *from_element_as_ptr(o);
	}

	const_pointer from_element_as_ptr(value_type o) const {
		return perms[orbit_.position(o)];
	}

	value_type predecessor(value_type o) const {
		return pred[orbit_.position(o)];
	}

	value_type get_root() const {
		return pred.front();
	}
private:
	mutable allocator alloc;
	orbit_type orbit_;
	std::vector<const_pointer> perms;
	std::vector<value_type> pred;
};

} // namespace perm_group

#endif /* PERM_GROUP_TRANSVERSAL_EXPLICIT_HPP */