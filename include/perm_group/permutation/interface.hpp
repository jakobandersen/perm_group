#ifndef PERM_GROUP_PERMUTATION_INTERFACE_HPP
#define PERM_GROUP_PERMUTATION_INTERFACE_HPP

#include <perm_group/permutation/traits.hpp>

#include <type_traits>

namespace perm_group {

template<typename Perm>
void put(Perm &p, typename permutation_traits<Perm>::value_type i, typename permutation_traits<Perm>::value_type image);
template<typename Perm>
std::size_t degree(const Perm &p);

// rst:
// rst: The following function templates are convenience functions and short-hands for using the operations on `permutation_traits`.
// rst: They constitute the basic generic interface for `Permutation` s.
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

namespace detail {

template<typename Perm, typename SrcPerm>
Perm copy_perm_dispatch(std::size_t n, SrcPerm &&p, std::true_type) {
	return Perm(std::forward<SrcPerm>(p));
}

template<typename Perm, typename SrcPerm>
Perm copy_perm_dispatch(std::size_t n, SrcPerm &&p, std::false_type) {
	Perm copy = perm_group::make_perm<Perm>(n);
	for(std::size_t i = 0; i != n; ++i)
		perm_group::put(copy, i, perm_group::get(p, i));
	return copy;
}

} // namespace detail

// rst: .. function:: template<typename Perm, typename SrcPerm> \
// rst:               Perm copy_perm(std::size_t n, SrcPerm &&p)
// rst:
// rst:		If `SrcPerm` and `Perm` are the same types, except for qualifies and referenceness,
// rst:		then the result is copy-constructed. Otherwise a new permutation is created with `make_perm`
// rst:		and initialized with `put`.
// rst:
// rst:		:returns: a copy of the given `Permutation` `p`.

template<typename Perm, typename SrcPerm>
Perm copy_perm(std::size_t n, SrcPerm &&p) {
	using IsSame = typename std::is_same<Perm, typename std::remove_const<typename std::remove_reference<SrcPerm>::type>::type>::type;
	return detail::copy_perm_dispatch<Perm>(n, std::forward<SrcPerm>(p), IsSame());
}

// rst: .. function:: template<typename Perm, typename SrcPerm> \
// rst:               Perm copy_perm(SrcPerm &&p)
// rst: 
// rst:		:returns: `perm_group::copy_perm<Perm>(perm_group::degree(p), std::forward<SrcPerm>(p))`

template<typename Perm, typename SrcPerm>
Perm copy_perm(SrcPerm &&p) {
	return perm_group::copy_perm<Perm>(perm_group::degree(p), std::forward<SrcPerm>(p));
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
// rst:
// rst:		Does `permutation_traits<Perm>::put(p, i, image)`.

template<typename Perm>
void put(Perm &p, typename permutation_traits<Perm>::value_type i, typename permutation_traits<Perm>::value_type image) {
	permutation_traits<Perm>::put(p, i, image);
}

// DegreeAwarePermutationConcept
//------------------------------------------------------------------------------

// rst: .. function:: template<typename Perm> \
// rst:               std::size_t degree(const Perm &p)
// rst:
// rst:		:returns: `permutation_traits<Perm>::degree(p)`

template<typename Perm>
std::size_t degree(const Perm &p) {
	return permutation_traits<Perm>::degree(p);
}

// Other
//------------------------------------------------------------------------------

// rst: .. function:: template<typename Perm> \
// rst:               Perm make_inverse(const Perm &p)
// rst:
// rst:		:returns: `permutation_traits<Perm>::make_inverse(p, perm_group::degree(p))`

template<typename Perm>
Perm make_inverse(const Perm &p) {
	return permutation_traits<Perm>::make_inverse(p, perm_group::degree(p));
}

} // namespace perm_group

#endif /* PERM_GROUP_PERMUTATION_INTERFACE_HPP */