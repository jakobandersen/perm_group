#ifndef PERM_GROUP_PERMUTATION_ARRAY_HPP
#define PERM_GROUP_PERMUTATION_ARRAY_HPP

#include <memory>

namespace perm_group {

template<typename ValueType>
struct array_permutation {
	using value_type = ValueType;

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