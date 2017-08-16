#ifndef PERM_GROUP_PERMUTATION_IO_HPP
#define PERM_GROUP_PERMUTATION_IO_HPP

#include <perm_group/io.hpp>
#include <perm_group/permutation/permutation.hpp>

#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>

namespace perm_group {
void check_element(std::size_t i, std::size_t n);
void check_set_hit(std::size_t i, std::vector<bool> &hit);

//------------------------------------------------------------------------------
// Read
//------------------------------------------------------------------------------

std::vector<std::vector<std::size_t> > read_permutation_cycles(std::istream &s);

template<typename Perm>
void read_permutation_cycles(std::istream &s, Perm &p, std::size_t n) {
	BOOST_CONCEPT_ASSERT((MutablePermutationConcept<Perm>));
	// reset to id, such that 'missing' cycles are correct
	for(std::size_t i = 0; i < n; ++i)
		put(p, i, i);
	auto cycles = read_permutation_cycles(s);
	std::vector<bool> hit(n, false);
	for(const auto &c : cycles) {
		assert(!c.empty());
		for(std::size_t i = 0; i + 1 < c.size(); ++i) {
			std::size_t dom = c[i];
			std::size_t img = c[i + 1];
			check_element(dom, n);
			check_element(img, n);
			check_set_hit(dom, hit);
			perm_group::put(p, dom, img);
		}
		// and the wrap-around
		std::size_t dom = c.back();
		std::size_t img = c.front();
		check_element(dom, n);
		check_set_hit(dom, hit);
		perm_group::put(p, dom, img);
	}
}

template<typename Perm>
void read_permutation_cycles(std::istream &s, Perm &p) {
	BOOST_CONCEPT_ASSERT((SizeAwarePermutationConcept<Perm>));
	read_permutation_cycles(s, p, perm_group::size(p));
}

template<typename Perm>
void read_permutation_cycles(const std::string &s, Perm &p, std::size_t n) {
	std::stringstream str(s);
	read_permutation_cycles(str, p, n);
}

template<typename Perm>
void read_permutation_cycles(const std::string &s, Perm &p) {
	BOOST_CONCEPT_ASSERT((SizeAwarePermutationConcept<Perm>));
	read_permutation_cycles(s, p, perm_group::size(p));
}

//------------------------------------------------------------------------------
// Write
//------------------------------------------------------------------------------

template<typename Perm>
std::ostream &write_permutation_cycles(std::ostream &s, const Perm &p, std::size_t n) {
	BOOST_CONCEPT_ASSERT((PermutationConcept<Perm>));
	std::vector<bool> printed(n, false);
	bool anyPrinted = false;
	for(std::size_t i = 0; i < n; ++i) {
		if(printed[i]) continue;
		if(perm_group::get(p, i) == i) continue;
		anyPrinted = true;
		std::size_t start = i;
		s << '(' << i;
		printed[i] = true;
		for(std::size_t next = perm_group::get(p, i); next != start; next = perm_group::get(p, next)) {
			assert(!printed[next]);
			printed[next] = true;
			s << ' ' << next;
		}
		s << ')';
	}
	if(!anyPrinted) s << "(0)";
	return s;
}

template<typename Perm>
std::ostream &write_permutation_cycles(std::ostream &s, const Perm &p) {
	BOOST_CONCEPT_ASSERT((SizeAwarePermutationConcept<Perm>));
	return write_permutation_cycles(s, p, perm_group::size(p));
}

} // namespace perm_group

#endif /* PERM_GROUP_PERMUTATION_IO_HPP */