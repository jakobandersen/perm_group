#ifndef PERM_GROUP_PERMUTATION_TRAITS_HPP
#define PERM_GROUP_PERMUTATION_TRAITS_HPP

#include <utility>

namespace perm_group {

template<typename Perm>
struct permutation_traits {
public: // PermutationConcept
	using value_type = typename Perm::value_type;

	static value_type get(const Perm &p, value_type i) {
		return p.get_(i);
	}

	static Perm make(std::size_t n) {
		return Perm(n);
	}

	static Perm make_identity(std::size_t n) {
		Perm p = make(n);
		for(std::size_t i = 0; i < n; ++i)
			put(p, i, i);
		return p;
	}

	static Perm copy(std::size_t n, Perm &&p) {
		return Perm(std::move(p));
	}

	static Perm copy(std::size_t n, const Perm &p) {
		return Perm(p);
	}

	template<typename UPerm>
	static Perm copy(std::size_t n, UPerm &&p) {
		Perm copy = make(n);
		for(std::size_t i = 0; i < n; ++i)
			put(copy, i, get(p, i));
		return copy;
	}
public: // MutablePermutationConcept

	static void put(Perm &p, value_type i, value_type image) {
		p.put_(i, image);
	}
public: // SizeAwarePermutationConcept

	static std::size_t size(const Perm &p) {
		return p.size_();
	}
public: // Other

	static Perm make_inverse(const Perm &p, std::size_t n) {
		Perm p_inv = make(n);
		for(std::size_t i = 0; i < n; ++i)
			put(p_inv, get(p, i), i);
		return p_inv;
	}
};

// Functions for making it easier to use
//==============================================================================

// PermutationConcept
//------------------------------------------------------------------------------

template<typename Perm>
typename permutation_traits<Perm>::value_type get(const Perm &p, typename permutation_traits<Perm>::value_type i) {
	return permutation_traits<Perm>::get(p, i);
}

template<typename Perm>
Perm make_perm(std::size_t n) {
	return permutation_traits<Perm>::make(n);
}

template<typename Perm, typename SrcPerm>
Perm copy_perm(std::size_t n, SrcPerm &&p) {
	return permutation_traits<Perm>::copy(n, std::forward<SrcPerm>(p));
}

template<typename Perm, typename SrcPerm>
Perm copy_perm(SrcPerm &&p) {
	return permutation_traits<Perm>::copy(size(p), std::forward<SrcPerm>(p));
}

template<typename Perm>
Perm make_identity_perm(std::size_t n) {
	return permutation_traits<Perm>::make_identity(n);
}

// MutablePermutationConcept
//------------------------------------------------------------------------------

template<typename Perm>
void put(Perm &p, typename permutation_traits<Perm>::value_type i, typename permutation_traits<Perm>::value_type image) {
	permutation_traits<Perm>::put(p, i, image);
}

// SizeAwarePermutationConcept
//------------------------------------------------------------------------------

template<typename Perm>
std::size_t size(const Perm &p) {
	return permutation_traits<Perm>::size(p);
}

// Other
//------------------------------------------------------------------------------

template<typename Perm>
Perm make_inverse(const Perm &p) {
	return permutation_traits<Perm>::make_inverse(p, perm_group::size(p));
}

} // namespace perm_group

#endif /* PERM_GROUP_PERMUTATION_TRAITS_HPP */