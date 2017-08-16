#ifndef PERM_GROUP_GROUP_BASIC_STABILIZER_HPP
#define PERM_GROUP_GROUP_BASIC_STABILIZER_HPP

#include <perm_group/group/group.hpp>
#include <perm_group/util/iterators.hpp>

#include <cassert>
#include <vector>

namespace perm_group {

//------------------------------------------------------------------------------
// Basic Stabilizer
//------------------------------------------------------------------------------

template<typename BaseGroup>
struct basic_stabilizer {
public: // Group
	using perm_type = typename BaseGroup::perm_type;
	using allocator = typename BaseGroup::allocator;
	using pointer = typename allocator::pointer;
	using const_pointer = typename allocator::const_pointer;
public: // Stabilizer
	using is_accurate = std::false_type;
public:
	using Store = std::vector<const_pointer>;
public:

	basic_stabilizer(std::size_t fixed, const BaseGroup &g) : g(g) {
		for(auto ptr : generator_ptrs(g)) {
			if(get(*ptr, fixed) == fixed)
				gen_set.push_back(ptr);
		}
	}

	basic_stabilizer(std::size_t fixed, const basic_stabilizer &g) : g(g.g) {
		for(auto ptr : generator_ptrs(g)) {
			if(get(*ptr, fixed) == fixed)
				gen_set.push_back(ptr);
		}
	}

public: // GroupConcept

	friend std::size_t degree(const basic_stabilizer &g) {
		return degree(g.g);
	}

	friend PtrContainerToPermProxy<Store, perm_type> generators(const basic_stabilizer &g) {
		return PtrContainerToPermProxy<Store, perm_type>(&g.gen_set);
	}

	friend PtrContainerToPtrProxy<Store, const_pointer> generator_ptrs(const basic_stabilizer &g) {
		return PtrContainerToPtrProxy<Store, const_pointer>(&g.gen_set);
	}

	friend allocator &get_allocator(const basic_stabilizer &g) {
		return get_allocator(g.g);
	}
protected:
	const BaseGroup &g;
	Store gen_set;
};


//------------------------------------------------------------------------------
// Basic Updatable Stabilizer
//------------------------------------------------------------------------------

template<typename BaseGroup>
struct basic_updatable_stabilizer : basic_stabilizer<BaseGroup> {
	using Base = basic_stabilizer<BaseGroup>;
	using perm_type = typename Base::perm_type;
public:

	basic_updatable_stabilizer(std::size_t fixed, const BaseGroup &g) : Base(fixed, g),
	fixed(fixed), parent(nullptr), parent_count(generators(g).size()), base_count(parent_count) { }

	basic_updatable_stabilizer(std::size_t fixed, basic_updatable_stabilizer &g) : Base(fixed, g),
	fixed(fixed), parent(&g), parent_count(parent->gen_set.size()), base_count(g.base_count) { }

	bool need_update() {
		return base_count != generators(this->g).size();
	}

	bool update() {
		if(!need_update()) return false;
		const auto f = [this](auto &&gens) {
			using std::begin;
			using std::end;
			bool updated = false;
			for(auto first = begin(gens) + parent_count; first != end(gens); ++first) {
				if(get(**first, fixed) == fixed) {
					updated = true;
					this->gen_set.push_back(*first);
				}
			}
			parent_count = gens.size();
			return updated;
		};
		if(parent) {
			parent->update();
			base_count = generators(this->g).size();
			return f(generator_ptrs(*parent));
		} else {
			base_count = generators(this->g).size();
			return f(generator_ptrs(this->g));
		}
	}
protected:
	std::size_t fixed;
	basic_updatable_stabilizer *parent;
	std::size_t parent_count, base_count;
};

} // namespace perm_group

#endif /* PERM_GROUP_GROUP_BASIC_STABILIZER_HPP */