#ifndef PERM_GROUP_PERMUTATION_BUILT_IN_HPP
#define PERM_GROUP_PERMUTATION_BUILT_IN_HPP

#include <perm_group/permutation/permutation.hpp>

#include <array>
#include <memory>
#include <type_traits>
#include <vector>

namespace perm_group {

// std::vector<T>
//------------------------------------------------------------------------------

template<typename T, typename Allocator>
struct permutation_traits<std::vector<T, Allocator> > {
	using Perm = std::vector<T, Allocator>;
public: // PermutationConcept
	using value_type = T;

	static value_type get(const Perm &p, value_type i) {
		return p[i];
	}
public: // MutablePermutationConcept

	static void put(Perm &p, value_type i, value_type image) {
		p[i] = image;
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
			put(copy, i, perm_group::get(p, i));
		return copy;
	}

public: // SizeAwarePermutationConcept

	static std::size_t size(const Perm &p) {
		return p.size();
	}
public: // Other

	static Perm make_inverse(const Perm &p, std::size_t n) {
		Perm p_inv = make(n);
		for(std::size_t i = 0; i < n; ++i)
			put(p_inv, get(p, i), i);
		return p_inv;
	}
};

// std::array<T, N>
//------------------------------------------------------------------------------

template<typename T, std::size_t N>
struct permutation_traits<std::array<T, N> > {
	using Perm = std::array<T, N>;
public: // PermutationConcept
	using value_type = T;

	static value_type get(const Perm &p, value_type i) {
		return p[i];
	}
public: // MutablePermutationConcept

	static void put(Perm &p, value_type i, value_type image) {
		p[i] = image;
	}

	static Perm make(std::size_t n) {
		return Perm();
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
public: // SizeAwarePermutationConcept

	static std::size_t size(const Perm &p) {
		return p.size();
	}
public: // Other

	static Perm make_inverse(const Perm &p, std::size_t n) {
		Perm p_inv = make(n);
		for(std::size_t i = 0; i < n; ++i)
			put(p_inv, get(p, i), i);
		return p_inv;
	}
};

} // namespace perm_group

#endif /* PERM_GROUP_PERMUTATION_BUILT_IN_HPP */