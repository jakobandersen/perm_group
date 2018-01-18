#ifndef PERM_GROUP_PERMUTATION_TRAITS_HPP
#define PERM_GROUP_PERMUTATION_TRAITS_HPP

#include <utility>

namespace perm_group {
namespace detail {

template<typename ...> // TODO: replace with std::void_t when switching to C++17
using void_t = void;

template<typename Perm, typename = void>
struct permutation_traits {
};

// rst: .. class:: template<typename Perm> \
// rst:            permutation_traits
// rst:

template<typename Perm>
struct permutation_traits<Perm, void_t<typename Perm::value_type> > {
	// rst:		.. type:: value_type = typename Perm::value_type
	// rst:
	// rst:			The whole specialization of this traits class only exists if this type exists.
	using value_type = typename Perm::value_type;
public: // PermutationConcept

	// rst:		.. function:: static value_type get(const Perm &p, value_type i)
	// rst:
	// rst:			:returns: the image of `i` under `p` using `p.get_(i)`.

	static value_type get(const Perm &p, value_type i) {
		return p.get_(i);
	}

	// rst:		.. function:: static Perm make(std::size_t n)
	// rst:
	// rst:			:returns: a new permutation of size `n` using `Perm(n)`.

	static Perm make(std::size_t n) {
		return Perm(n);
	}

	// rst:		.. function:: static Perm make_identity(std::size_t n)
	// rst:
	// rst:			:returns: a permutation created with `make` and initialized as the identity.

	static Perm make_identity(std::size_t n) {
		Perm p = make(n);
		for(std::size_t i = 0; i < n; ++i)
			put(p, i, i);
		return p;
	}

	// rst:		.. function:: static Perm copy(std::size_t n, Perm &&p)
	// rst:
	// rst:			:returns: `Perm(std::move(p))`

	static Perm copy(std::size_t n, Perm &&p) {
		return Perm(std::move(p));
	}

	// rst:		.. function:: static Perm copy(std::size_t n, const Perm &p)
	// rst:
	// rst:			:returns: `Perm(p)`

	static Perm copy(std::size_t n, const Perm &p) {
		return Perm(p);
	}

	// rst:		.. function:: template<typename UPerm> \
	// rst:		              static Perm copy(std::size_t n, UPerm &&p)
	// rst:
	// rst:			:returns: a permutation created with `make` where `p` has been copied into.

	template<typename UPerm>
	static Perm copy(std::size_t n, UPerm &&p) {
		Perm copy = make(n);
		for(std::size_t i = 0; i < n; ++i)
			put(copy, i, get(p, i));
		return copy;
	}
public: // MutablePermutationConcept

	// rst:		.. function:: static void put(Perm &p, value_type i, value_type image)
	// rst:
	// rst:			Sets the image of `i` to `image` using `p.put_(i, image)`.

	static void put(Perm &p, value_type i, value_type image) {
		p.put_(i, image);
	}
public: // SizeAwarePermutationConcept

	// rst:		.. function:: static std::size_t size(const Perm &p)
	// rst:
	// rst:			:returns: `p.size_()`

	static std::size_t size(const Perm &p) {
		return p.size_();
	}
public: // Other
	// rst:		.. function:: static Perm make_inverse(const Perm &p, std::size_t n)
	// rst:
	// rst:			:returns: a permutation created with `make` where the inverse of `p` has been copied into.

	static Perm make_inverse(const Perm &p, std::size_t n) {
		Perm p_inv = make(n);
		for(std::size_t i = 0; i < n; ++i)
			put(p_inv, get(p, i), i);
		return p_inv;
	}
};

} // namespace detail

template<typename Perm>
struct permutation_traits : detail::permutation_traits<Perm> {
};

// Functions for making it easier to use
//==============================================================================

// PermutationConcept
//------------------------------------------------------------------------------

// rst:
// rst: Permutation Interface
// rst: =====================
// rst:
// rst: The following function templates are short-hands for using the operations on `permutation_traits`.
// rst:
// rst: .. function:: template<typename Perm> \
// rst:               typename permutation_traits<Perm>::value_type get(const Perm &p, typename permutation_traits<Perm>::value_type i)
// rst:
// rst:		:returns: `permutation_traits<Perm>::get(p, i)`

template<typename Perm>
typename permutation_traits<Perm>::value_type get(const Perm &p, typename permutation_traits<Perm>::value_type i) {
	return permutation_traits<Perm>::get(p, i);
}

// rst: .. function:: template<typename Perm> \
// rst:               Perm make_perm(std::size_t n)
// rst:
// rst:		:returns: `permutation_traits<Perm>::make(n)`

template<typename Perm>
Perm make_perm(std::size_t n) {
	return permutation_traits<Perm>::make(n);
}

// rst: .. function:: template<typename Perm, typename SrcPerm> \
// rst:               Perm copy_perm(std::size_t n, SrcPerm &&p)
// rst:
// rst:		:returns: `permutation_traits<Perm>::copy(n, std::forward<SrcPerm>(p))`

template<typename Perm, typename SrcPerm>
Perm copy_perm(std::size_t n, SrcPerm &&p) {
	return permutation_traits<Perm>::copy(n, std::forward<SrcPerm>(p));
}
// rst: .. function:: template<typename Perm, typename SrcPerm> \
// rst:               Perm copy_perm(SrcPerm &&p)
// rst: 
// rst:		:returns: `permutation_traits<Perm>::copy(size(p), std::forward<SrcPerm>(p))`

template<typename Perm, typename SrcPerm>
Perm copy_perm(SrcPerm &&p) {
	return permutation_traits<Perm>::copy(size(p), std::forward<SrcPerm>(p));
}

// rst: .. function:: template<typename Perm> \
// rst:               Perm make_identity_perm(std::size_t n)
// rst:
// rst:		:returns: `permutation_traits<Perm>::make_identity(n)`

template<typename Perm>
Perm make_identity_perm(std::size_t n) {
	return permutation_traits<Perm>::make_identity(n);
}

// MutablePermutationConcept
//------------------------------------------------------------------------------

// rst: .. function:: template<typename Perm> \
// rst:               void put(Perm &p, typename permutation_traits<Perm>::value_type i, typename permutation_traits<Perm>::value_type image)

template<typename Perm>
void put(Perm &p, typename permutation_traits<Perm>::value_type i, typename permutation_traits<Perm>::value_type image) {
	permutation_traits<Perm>::put(p, i, image);
}

// SizeAwarePermutationConcept
//------------------------------------------------------------------------------

// rst: .. function:: template<typename Perm> \
// rst:               std::size_t size(const Perm &p)
// rst:
// rst:		:returns: `permutation_traits<Perm>::size(p)`

template<typename Perm>
std::size_t size(const Perm &p) {
	return permutation_traits<Perm>::size(p);
}

// Other
//------------------------------------------------------------------------------

// rst: .. function:: template<typename Perm> \
// rst:               Perm make_inverse(const Perm &p)
// rst:
// rst:		:returns: `permutation_traits<Perm>::make_inverse(p, perm_group::size(p))`

template<typename Perm>
Perm make_inverse(const Perm &p) {
	return permutation_traits<Perm>::make_inverse(p, perm_group::size(p));
}

} // namespace perm_group

#endif /* PERM_GROUP_PERMUTATION_TRAITS_HPP */