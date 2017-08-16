#ifndef PERM_GROUP_GROUP_SCHREIER_STABILIZER_HPP
#define PERM_GROUP_GROUP_SCHREIER_STABILIZER_HPP

#include <perm_group/group/group.hpp>
#include <perm_group/permutation/mult.hpp>
#include <perm_group/util/iterators.hpp>

#include <cassert>
#include <vector>

namespace perm_group {

//------------------------------------------------------------------------------
// Schreier Stabilizer
//------------------------------------------------------------------------------

template<typename BaseGroup, typename Transversal>
struct schreier_stabilizer {
public: // Group
	using perm_type = typename BaseGroup::perm_type;
	using allocator = typename BaseGroup::allocator;
	using pointer = typename allocator::pointer;
	using const_pointer = typename allocator::const_pointer;
public: // Stabilizer
	using is_accurate = std::true_type;
public:
	using Store = std::vector<const_pointer>;
protected:

	template<typename GenPtrIter>
	void update(std::size_t fixed, Transversal &trans, GenPtrIter first, GenPtrIter lastOld, GenPtrIter last) {
		const auto onNewElement = [&](const auto &u, const auto &u_img, const auto &gen_iter, const_pointer trans_u_img) {
			const_pointer trans_u_img_inv = get_allocator(g).copy(degree(g), make_inverse(*trans_u_img));
			inverse_trans.push_back(trans_u_img_inv);
		};
		const auto onDupElement = [&](const auto &u, const auto &u_img, const auto &gen_iter, const_pointer trans_u_img) {
			const_pointer trans_u = trans.from_element_as_ptr(u);
			const_pointer trans_u_img_inv = inverse_trans[orbit(trans).position(u_img)];
			// Add T[u] * g * T[g(u)]^- as a generator.
			auto gen_expr = perm_group::mult(perm_group::mult(*trans_u, **gen_iter), *trans_u_img_inv);
			// check if we already have it
			const bool isDup = [&]() {
				for(const_pointer gen : gen_set) {
					bool equal = true;
					for(std::size_t i = 0; i < degree(g); ++i) {
						if(get(*gen, i) != get(gen_expr, i)) {
							equal = false;
							break;
						}
					}
					if(equal) return true;
				}
				return false;
			}();
			if(!isDup) {
				const_pointer gen = get_allocator(g).copy(degree(g), gen_expr);
				gen_set.push_back(gen);
			}
		};
		trans.update(first, lastOld, last, onNewElement, onDupElement);
	}
private:

	template<typename Group>
	void commonInit(std::size_t fixed, const Group &g) {
		Transversal trans(degree(g), fixed, get_allocator(g));
		const auto gs = generator_ptrs(g);
		update(fixed, trans, gs.begin(), gs.begin(), gs.end());
	}
protected:

	schreier_stabilizer(const BaseGroup &g) : g(g) {
		inverse_trans.push_back(get_allocator(*this).make_identity(degree(*this)));
	}
public:

	schreier_stabilizer(std::size_t fixed, const BaseGroup &g) : schreier_stabilizer(g) {
		commonInit(fixed, g);
	}

	schreier_stabilizer(std::size_t fixed, const schreier_stabilizer &g) : schreier_stabilizer(g.g) {
		commonInit(fixed, g);
	}

	~schreier_stabilizer() {
		for(std::size_t i = 0; i < gen_set.size(); ++i)
			get_allocator(*this).release(degree(*this), std::move(gen_set[i]));
		for(std::size_t i = 0; i < inverse_trans.size(); ++i)
			get_allocator(*this).release(degree(*this), std::move(inverse_trans[i]));
	}
public: // GroupConcept

	friend std::size_t degree(const schreier_stabilizer &g) {
		return degree(g.g);
	}

	friend PtrContainerToPermProxy<Store, perm_type> generators(const schreier_stabilizer &g) {
		return PtrContainerToPermProxy<Store, perm_type>(&g.gen_set);
	}

	friend PtrContainerToPtrProxy<Store, const_pointer> generator_ptrs(const schreier_stabilizer &g) {
		return PtrContainerToPtrProxy<Store, const_pointer>(&g.gen_set);
	}

	friend allocator &get_allocator(const schreier_stabilizer &g) {
		return get_allocator(g.g);
	}
protected:
	const BaseGroup &g;
	Store inverse_trans;
	Store gen_set;
};


//------------------------------------------------------------------------------
// Schreier Updatable Stabilizer
//------------------------------------------------------------------------------

template<typename BaseGroup, typename Transversal>
struct schreier_updatable_stabilizer : schreier_stabilizer<BaseGroup, Transversal> {
	using Base = schreier_stabilizer<BaseGroup, Transversal>;
	using perm_type = typename Base::perm_type;
private:

	schreier_updatable_stabilizer(std::size_t fixed, const BaseGroup &g, schreier_updatable_stabilizer *parent)
	: Base(g), fixed(fixed), parent(parent), parent_count(0)
	, trans(degree(g), fixed, get_allocator(g)) {
		update();
	}
public:

	schreier_updatable_stabilizer(std::size_t fixed, const BaseGroup &g)
	: schreier_updatable_stabilizer(fixed, g, nullptr) { }

	schreier_updatable_stabilizer(std::size_t fixed, schreier_updatable_stabilizer &g)
	: schreier_updatable_stabilizer(fixed, g.g, &g) { }

	void update() {
		const auto f = [this](auto &&gs) {
			const auto first = gs.begin();
			const auto lastOld = first + parent_count;
			Base::update(fixed, trans, first, lastOld, gs.end());
			parent_count = gs.size();
		};
		if(parent) {
			parent->update();
			return f(generator_ptrs(*parent));
		} else {
			return f(generator_ptrs(this->g));
		}
	}
protected:
	std::size_t fixed;
	schreier_updatable_stabilizer *parent;
	std::size_t parent_count;
	Transversal trans;
};

} // namespace perm_group

#endif /* PERM_GROUP_GROUP_SCHREIER_STABILIZER_HPP */