#ifndef PERM_GROUP_PERMUTATION_MULT_HPP
#define PERM_GROUP_PERMUTATION_MULT_HPP

#include <perm_group/permutation/permutation.hpp>

#include <type_traits>

namespace perm_group {

template<typename PermL, typename PermR>
struct mult_expr {
	using PermL_ = typename std::remove_reference<PermL>::type;
	using PermR_ = typename std::remove_reference<PermR>::type;
	using value_type = typename permutation_traits<PermL_>::value_type;
public:

	mult_expr(PermL left, PermR right) : left(left), right(right) { }
public: // PermutationConcept

	value_type get_(value_type i) const {
		return get(right, get(left, i));
	}
public: // DegreeAwarePermutationConcept

	std::size_t degree_() const {
		return degree(this->left);
	}
public:

	template<typename Perm, typename = typename std::enable_if<!std::is_same<Perm, mult_expr>::value>::type>
	// requires DegreeAwarePermutationConcept<PermL>
	operator Perm() const {
		return copy_perm<Perm>(*this);
	}
protected:
	PermL left;
	PermR right;
};

// rst: .. function:: template<typename PermL, typename PermR> \
// rst:               mult_expr<PermL, PermR> mult(PermL &&left, PermR &&right)
// rst:
// rst:		:returns: an expression template that acts as `Permutation`.

template<typename PermL, typename PermR>
mult_expr<PermL, PermR> mult(PermL &&left, PermR &&right) {
	return mult_expr<PermL, PermR>(std::forward<PermL>(left), std::forward<PermR>(right));
}

} // namespace perm_group

#endif /* PERM_GROUP_PERMUTATION_MULT_HPP */