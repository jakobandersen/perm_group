#ifndef PERM_GROUP_GROUP_STABILIZER_CHAIN_HPP
#define PERM_GROUP_GROUP_STABILIZER_CHAIN_HPP

#include <perm_group/group/generated.hpp>
#include <perm_group/group/schreier_stabilizer.hpp>

#include <vector>

namespace perm_group {

template<typename Perm, typename Transversal, typename Alloc = default_allocator<Perm>>
struct stabilizer_chain {
	using value_type = typename permutation_traits<Perm>::value_type;
	using base_group = generated_group<Perm, Alloc>;
	using stabilizer = schreier_updatable_stabilizer<base_group, Transversal>;

	struct chain_element {
		value_type fixed;
		std::unique_ptr<stabilizer> stab;
	};
public:

	stabilizer_chain(std::size_t n, Alloc alloc = Alloc()) : g(n, alloc) { }
	stabilizer_chain(stabilizer_chain&&) = delete;
	stabilizer_chain &operator=(stabilizer_chain&&) = delete;

	void add_generator(const Perm &perm) {
		assert(false);
	}
public: // GroupConcept

	friend decltype(auto) degree(const generated_group &g) {
		return degree(g.g);
	}

	friend decltype(auto) generators(const generated_group &g) {
		return generators(g.g);
	}

	friend decltype(auto) generator_ptrs(const generated_group &g) {
		return generator_ptrs(g.g);
	}

	friend decltype(auto) get_allocator(const generated_group &g) {
		return get_allocator(g.g);
	}
private:
	base_group g;
	std::vector<chain_element> chain;
};

} // namespace perm_group

#endif /* PERM_GROUP_GROUP_STABILIZER_CHAIN_HPP */