#ifndef PERM_GROUP_PERMUTATION_ARRAY_HPP
#define PERM_GROUP_PERMUTATION_ARRAY_HPP

#include <memory>

namespace perm_group {

// rst: .. class:: template<typename ValueType> \
// rst:            array_permutation
// rst:
// rst:		Models `MutablePermutation` (but not `DegreeAwarePermutation`).
// rst:
// rst:		A minimum overhead permutation implemented as a raw pointer to array.
// rst:

template<typename ValueType>
struct array_permutation {
	using value_type = ValueType;

	// rst:		.. function:: array_permutation(std::size_t n)
	// rst:
	// rst:			Construct a non-initialized permutation of size `n`.

	array_permutation(std::size_t n) : p(new value_type[n]) { }

	value_type get_(value_type i) const {
		return p[i];
	}

	void put_(value_type i, value_type image) {
		p[i] = image;
	}
private:
	std::unique_ptr<value_type[] > p;
};

} // namespace perm_group

#endif /* PERM_GROUP_PERMUTATION_ARRAY_HPP */