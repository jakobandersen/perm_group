#ifndef PERM_GROUP_PERMUTATION_IO_HPP
#define PERM_GROUP_PERMUTATION_IO_HPP

#include <perm_group/io.hpp>
#include <perm_group/permutation/permutation.hpp>

#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>

namespace perm_group {
namespace detail {
void check_element(std::size_t i, std::size_t n);
void check_set_hit(std::size_t i, std::vector<bool> &hit);
} // namespace detail

//------------------------------------------------------------------------------
// Read
//------------------------------------------------------------------------------

// rst: .. function:: std::vector<std::vector<std::size_t> > read_permutation_cycles(std::istream &s)
// rst:
// rst:		:returns: a list of cycles of non-negative numbers parsed from the given stream. It does not necessarily represent a permutation (see the other overload).
// rst:		:throws: `io_error` on parsing errors.

std::vector<std::vector<std::size_t> > read_permutation_cycles(std::istream &s);

// rst: .. function:: template<typename Perm> \
// rst:               void read_permutation_cycles(std::istream &s, Perm &p, std::size_t n)
// rst:               template<typename Perm> \
// rst:               void read_permutation_cycles(std::istream &s, Perm &p)
// rst:               template<typename Perm> \
// rst:               void read_permutation_cycles(const std::string &s, Perm &p, std::size_t n)
// rst:               template<typename Perm> \
// rst:               void read_permutation_cycles(const std::string &s, Perm &p)
// rst:
// rst:		Parse a permutation in cycle notation from a stream or a string (`s`) and store it in `p`.
// rst:		Permutations are zero-indexed.
// rst:
// rst:		The overloads without `n` requires `SizeAwarePermutation<Perm>`,
// rst:		and sets `n = perm_group::size(p)`.
// rst:
// rst:		:throws: `io_error` if parsing fails.
// rst:		:throws: `io_error` if a number is `n` or higher.
// rst:		:throws: `io_error` if the data does not represent a permutation.

template<typename Perm>
void read_permutation_cycles(std::istream &s, Perm &p, std::size_t n) {
	BOOST_CONCEPT_ASSERT((MutablePermutation<Perm>));
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
			detail::check_element(dom, n);
			detail::check_element(img, n);
			detail::check_set_hit(dom, hit);
			perm_group::put(p, dom, img);
		}
		// and the wrap-around
		std::size_t dom = c.back();
		std::size_t img = c.front();
		detail::check_element(dom, n);
		detail::check_set_hit(dom, hit);
		perm_group::put(p, dom, img);
	}
}

template<typename Perm>
void read_permutation_cycles(std::istream &s, Perm &p) {
	BOOST_CONCEPT_ASSERT((SizeAwarePermutation<Perm>));
	read_permutation_cycles(s, p, perm_group::size(p));
}

template<typename Perm>
void read_permutation_cycles(const std::string &s, Perm &p, std::size_t n) {
	std::stringstream str(s);
	read_permutation_cycles(str, p, n);
}

template<typename Perm>
void read_permutation_cycles(const std::string &s, Perm &p) {
	BOOST_CONCEPT_ASSERT((SizeAwarePermutation<Perm>));
	read_permutation_cycles(s, p, perm_group::size(p));
}

//------------------------------------------------------------------------------
// Write
//------------------------------------------------------------------------------

// rst: .. function:: template<typename Perm> \
// rst:	              std::ostream &write_permutation_cycles(std::ostream &s, const Perm &p, std::size_t n)
// rst:
// rst:		Write a permutation in cycle notation.

template<typename Perm>
std::ostream &write_permutation_cycles(std::ostream &s, const Perm &p, std::size_t n) {
	BOOST_CONCEPT_ASSERT((Permutation<Perm>));
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

// rst: .. function:: template<typename Perm> \
// rst:	              std::ostream &write_permutation_cycles(std::ostream &s, const Perm &p)
// rst:
// rst:		Requires `SizeAwarePermutation<Perm>`.
// rst:		
// rst:		Write a permutation in cycle notation.

template<typename Perm>
std::ostream &write_permutation_cycles(std::ostream &s, const Perm &p) {
	BOOST_CONCEPT_ASSERT((SizeAwarePermutation<Perm>));
	return write_permutation_cycles(s, p, perm_group::size(p));
}

} // namespace perm_group

#endif /* PERM_GROUP_PERMUTATION_IO_HPP */