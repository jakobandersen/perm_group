#ifndef PERM_GROUP_PERMUTATION_PERMUTATION_HPP
#define PERM_GROUP_PERMUTATION_PERMUTATION_HPP

#include <perm_group/permutation/traits.hpp>

#include <boost/concept_check.hpp>

namespace perm_group {

//------------------------------------------------------------------------------
// Concepts
//------------------------------------------------------------------------------

template<typename Perm>
struct PermutationConcept {
	using value_type = typename permutation_traits<Perm>::value_type;
	// Perm must be MoveConstructible and MoveAssignable

	BOOST_CONCEPT_USAGE(PermutationConcept) {
		const Perm &pConst = p;
		i = perm_group::get(pConst, i);
	}
private:
	Perm p;
	value_type i;
};

template<typename Perm>
struct MutablePermutationConcept {
	BOOST_CONCEPT_ASSERT((PermutationConcept<Perm>));
	using value_type = typename permutation_traits<Perm>::value_type;

	BOOST_CONCEPT_USAGE(MutablePermutationConcept) {
		perm_group::put(p, i, i);
		Perm pNew = perm_group::make_perm<Perm>(i);
		pNew = perm_group::make_identity_perm<Perm>(i);
	}
private:
	Perm p;
	value_type i;
};

template<typename Perm>
struct SizeAwarePermutationConcept {
	BOOST_CONCEPT_ASSERT((PermutationConcept<Perm>));

	BOOST_CONCEPT_USAGE(SizeAwarePermutationConcept) {
		const Perm &pConst = p;
		std::size_t s = perm_group::size(pConst);
		(void) s;
	}
private:
	Perm p;
};

} // namespace perm_group

#endif /* PERM_GROUP_PERMUTATION_PERMUTATION_HPP */