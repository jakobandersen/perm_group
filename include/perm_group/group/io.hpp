#ifndef PERM_GROUP_GROUP_IO_HPP
#define PERM_GROUP_GROUP_IO_HPP

#include <perm_group/group/group.hpp>
#include <perm_group/permutation/io.hpp>

namespace perm_group {

//------------------------------------------------------------------------------
// Write
//------------------------------------------------------------------------------

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

template<typename Group>
std::ostream &write_group(std::ostream &s, const Group &g) {
	BOOST_CONCEPT_ASSERT((GroupConcept<Group>));
	using std::begin;
	using std::end;
	auto gens = generators(g);
	return write_generating_set(s, begin(gens), end(gens));
}

} // namespace perm_group

#endif /* PERM_GROUP_GROUP_IO_HPP */