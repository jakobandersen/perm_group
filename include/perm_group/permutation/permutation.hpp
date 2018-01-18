#ifndef PERM_GROUP_PERMUTATION_PERMUTATION_HPP
#define PERM_GROUP_PERMUTATION_PERMUTATION_HPP

#include <perm_group/permutation/traits.hpp>

#include <boost/concept_check.hpp>

namespace perm_group {

//------------------------------------------------------------------------------
// Concepts
//------------------------------------------------------------------------------

// rst: .. concept:: template<typename Perm> \
// rst:              Permutation
// rst:
// rst:		Requires `MoveConstructible<Perm>` and `MoveAssignable<Perm>`.
// rst:
// rst:		.. assoc_types::
// rst:
// rst:		.. type:: value_type = typename permutation_traits<Perm>::value_type
// rst:
// rst:			The type of objects the permutation maps.
// rst:
// rst:		.. notation::
// rst:
// rst:		.. var:: Perm p
// rst:		.. var:: value_type i
// rst:
// rst:		.. valid_expr::
// rst:
// rst:		- `perm_group::get(p, i)`: returns the image of `i` under `p`.
// rst:		  The return type must be convertible to `value_type`.

template<typename Perm>
struct Permutation {
	using value_type = typename permutation_traits<Perm>::value_type;

	BOOST_CONCEPT_USAGE(Permutation) {
		const Perm &pConst = p;
		i = perm_group::get(pConst, i);
	}
private:
	Perm p;
	value_type i;
};

// rst: .. concept:: template<typename Perm> \
// rst:              MutablePermutation
// rst:
// rst:		Requires `Permutation<Perm>`.
// rst:
// rst:		.. assoc_types::
// rst:
// rst:		.. type:: value_type = typename permutation_traits<Perm>::value_type
// rst:
// rst:			The type of objects the permutation maps.
// rst:
// rst:		.. notation::
// rst:
// rst:		.. var:: Perm p
// rst:		         value_type i
// rst:		         value_type j
// rst:
// rst:		.. valid_expr::
// rst:
// rst:		- `perm_group::put(p, i, j)`: sets the image of `i` under `p` to `j`.

template<typename Perm>
struct MutablePermutation {
	BOOST_CONCEPT_ASSERT((Permutation<Perm>));
	using value_type = typename permutation_traits<Perm>::value_type;

	BOOST_CONCEPT_USAGE(MutablePermutation) {
		perm_group::put(p, i, i);
		Perm pNew = perm_group::make_perm<Perm>(i);
		pNew = perm_group::make_identity_perm<Perm>(i);
	}
private:
	Perm p;
	value_type i;
};

// rst: .. concept:: template<typename Perm> \
// rst:              SizeAwarePermutation
// rst:
// rst:		Requires `Permutation<Perm>`.
// rst:
// rst:		.. notation::
// rst:
// rst:		.. var:: Perm p
// rst:
// rst:		.. valid_expr::
// rst:
// rst:		- `perm_group::size(p)`: returns the size of the permutation.
// rst:		  The return type must be convertible to `std::size_t`.

template<typename Perm>
struct SizeAwarePermutation {
	BOOST_CONCEPT_ASSERT((Permutation<Perm>));

	BOOST_CONCEPT_USAGE(SizeAwarePermutation) {
		const Perm &pConst = p;
		std::size_t s = perm_group::size(pConst);
		(void) s;
	}
private:
	Perm p;
};

} // namespace perm_group

#endif /* PERM_GROUP_PERMUTATION_PERMUTATION_HPP */