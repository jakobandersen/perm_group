#ifndef PERM_GROUP_PERMUTATION_WORD_HPP
#define PERM_GROUP_PERMUTATION_WORD_HPP

#include <perm_group/permutation/traits.hpp>

#include <memory>
#include <vector>

namespace perm_group {
namespace detail {

template<typename Pointer, typename ValueType>
ValueType permutation_word_get(const Pointer *first, const Pointer *last, ValueType v) {
	constexpr int unfold = 8;
	const auto len = last - first;
	const auto rounds = len / unfold;
	const auto rest = len % unfold;
	for(int round = 0; round != rounds; ++round)
		for(int j = 0; j != unfold; ++j)
			v = perm_group::get(**(first + round * unfold + j), v);
	for(int j = 0; j != rest; ++j)
		v = perm_group::get(**(first + rounds * unfold + j), v);
	return v;
}

} // namespace detail

template<typename Pointer>
struct permutation_word {
	using perm = typename std::pointer_traits<Pointer>::element_type;
	using value_type = typename permutation_traits<perm>::value_type;

	permutation_word() = default;

	permutation_word(std::initializer_list<Pointer> ps) {
		perms.insert(perms.begin(), ps.begin(), ps.end());
		for(Pointer p : perms) assert(p);
	}

	void push_back(Pointer p) {
		perms.push_back(p);
	}

	void clear() {
		perms.clear();
	}

	bool empty() const {
		return perms.empty();
	}

	std::size_t size() const {
		return perms.size();
	}
public:

	Pointer operator[](std::size_t i) const {
		return perms[i];
	}

	value_type get_(value_type i) const {
		const auto *first = &perms[0];
		const auto *last = first + perms.size();
		return detail::permutation_word_get(first, last, i);
	}
private:
	std::vector<Pointer> perms;
};

template<typename Pointer>
struct permutation_word_fixed {
	using perm = typename std::pointer_traits<Pointer>::element_type;
	using value_type = typename permutation_traits<perm>::value_type;

	permutation_word_fixed() = default;

	explicit permutation_word_fixed(std::size_t n)
	: perms(new Pointer[n]), next(perms.get()) { }

	void reserve(std::size_t n) {
		perms.reset(new Pointer[n]);
		next = perms.get();
	}

	void push_back(Pointer p) {
		*next = p;
		++next;
	}

	void clear() {
		next = perms.get();
	}

	bool empty() const {
		return next == perms.get();
	}

	std::size_t size() const {
		return next - perms.get();
	}
public:

	Pointer operator[](std::size_t i) const {
		return perms[i];
	}

	value_type get_(value_type i) const {
		return detail::permutation_word_get(perms.get(), next, i);
	}
private:
	std::unique_ptr<Pointer[] > perms;
	Pointer *next = nullptr;
};

} // namespace perm_group

#endif /* PERM_GROUP_PERMUTATION_WORD_HPP */
