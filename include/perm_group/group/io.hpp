#ifndef PERM_GROUP_GROUP_IO_HPP
#define PERM_GROUP_GROUP_IO_HPP

#include <perm_group/group/group.hpp>
#include <perm_group/permutation/io.hpp>

namespace perm_group {

//------------------------------------------------------------------------------
// Write
//------------------------------------------------------------------------------

// rst: .. function:: template<typename Iter> \
// rst:               std::ostream &write_generating_set(std::ostream &s, Iter first, const Iter &last)
// rst:
// rst:		Write the range of permutations to the given stream, formatted as a generating set.
// rst:		E.g., ``<(0), (0 1 2 3)>``.
// rst:

template<typename Iter>
std::ostream &write_generating_set(std::ostream &s, Iter first, const Iter &last) {
	s << "<";
	bool has_printed = false;
	for(; first != last; ++first) {
		if(has_printed) s << ", ";
		has_printed = true;
		write_permutation_cycles(s, *first);
	}
	return s << ">";
}

// rst: .. function:: template<typename GroupT> \
// rst:               std::ostream &write_group(std::ostream &s, const GroupT &g)
// rst:
// rst:		A helper function using `write_generating_set` to write the given `Group`.
// rst:

template<typename GroupT>
std::ostream &write_group(std::ostream &s, const GroupT &g) {
	BOOST_CONCEPT_ASSERT((Group<GroupT>));
	using std::begin;
	using std::end;
	auto gens = g.generators();
	return write_generating_set(s, begin(gens), end(gens));
}

} // namespace perm_group

#endif /* PERM_GROUP_GROUP_IO_HPP */