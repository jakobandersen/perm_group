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
public: // MutablePermutationConcept

	// rst:		.. function:: static void put(Perm &p, value_type i, value_type image)
	// rst:
	// rst:			Sets the image of `i` to `image` using `p.put_(i, image)`.

	static void put(Perm &p, value_type i, value_type image) {
		p.put_(i, image);
	}
public: // DegreeAwarePermutationConcept

	// rst:		.. function:: static std::size_t degree(const Perm &p)
	// rst:
	// rst:			:returns: `p.degree_()`

	static std::size_t degree(const Perm &p) {
		return p.degree_();
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

} // namespace perm_group

#endif /* PERM_GROUP_PERMUTATION_TRAITS_HPP */