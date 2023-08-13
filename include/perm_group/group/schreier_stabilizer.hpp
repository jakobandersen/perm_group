#ifndef PERM_GROUP_GROUP_SCHREIER_STABILIZER_HPP
#define PERM_GROUP_GROUP_SCHREIER_STABILIZER_HPP

#include <perm_group/group/group.hpp>
#include <perm_group/permutation/mult.hpp>
#include <perm_group/transversal/transversal.hpp>
#include <perm_group/util/iterators.hpp>

#include <cassert>
#include <vector>

namespace perm_group {

// rst: .. todo:: Actually write the documentation for this file.

//------------------------------------------------------------------------------
// Schreier Stabilizer
//------------------------------------------------------------------------------

template<typename Transversal, typename DupChecker = DupCheckerNop>
struct schreier_stabilizer {
	BOOST_CONCEPT_ASSERT((TransversalConcept<Transversal>));
public: // Group
	using allocator = typename Transversal::allocator;
	using perm = typename allocator::perm;
	BOOST_CONCEPT_ASSERT((boost::EqualityComparable<perm>));
	using pointer = typename allocator::pointer;
	using const_pointer = typename allocator::const_pointer;
public: // Stabilizer
	using is_accurate = std::true_type;
public:
	using Store = std::vector<const_pointer>;
public:

	schreier_stabilizer(std::size_t fixed, const allocator &alloc, DupChecker dupChecker = DupChecker())
	: trans(fixed, alloc), dupChecker(dupChecker) {
		const_pointer p = get_allocator().make_identity();
		gen_set.push_back(p);
		inverse_trans.push_back(p);
	}

	schreier_stabilizer(schreier_stabilizer &&other)
	: schreier_stabilizer(other.fixed_element(), other.get_allocator(), other.dupChecker) {
		// steal everything they got, but give them our newly created stuff
		using std::swap;
		swap(trans, other.trans);
		swap(inverse_trans, other.inverse_trans);
		swap(gen_set, other.gen_set);
	};

	schreier_stabilizer &operator=(schreier_stabilizer &&other) {
		using std::swap;
		swap(trans, other.trans);
		swap(inverse_trans, other.inverse_trans);
		swap(gen_set, other.gen_set);
		swap(dupChecker, other.dupChecker);
		return *this;
	}

	schreier_stabilizer(const schreier_stabilizer&) = delete;
	schreier_stabilizer &operator=(const schreier_stabilizer&) = delete;

	~schreier_stabilizer() {
		// note: the id permutation is shared among the two
		for(std::size_t i = 0; i != gen_set.size(); ++i)
			get_allocator().release(gen_set[i]);
		for(std::size_t i = 1; i != inverse_trans.size(); ++i)
			get_allocator().release(inverse_trans[i]);
	}
public: // GroupConcept

	std::size_t degree() const {
		return get_allocator().degree();
	}

	PtrContainerToPermProxy<Store, perm> generators() const {
		return PtrContainerToPermProxy<Store, perm>(gen_set);
	}

	PtrContainerToPtrProxy<Store, const_pointer> generator_ptrs() const {
		return PtrContainerToPtrProxy<Store, const_pointer>(gen_set);
	}

	decltype(auto) get_allocator() const {
		return trans.get_allocator();
	}
public: // StabilizerConcept

	std::size_t fixed_element() const {
		return trans.get_root();
	}

	template<typename Iter>
	void add_generators(Iter first, Iter lastOld, Iter lastNew) {
		add_generators(first, lastOld, lastNew, [](auto&&... args) {
		});
	}

	template<typename Iter, typename Next>
	void add_generators(Iter first, Iter lastOld, Iter lastNew, Next next) {
		// The 'this->' part is needed due to bugs in GCC 6:
		// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67274
		const auto onNewElement = [&](const auto &u, const auto &u_img, const auto &gen_iter, const_pointer trans_u_img) {
			const_pointer trans_u_img_inv = this->get_allocator().copy(perm_group::make_inverse(*trans_u_img));
			assert(perm_group::get(*trans_u_img_inv, u_img) == trans.get_root());
			inverse_trans.push_back(trans_u_img_inv);
		};
		const auto onDupElement = [&, root = trans.get_root()](const auto &u, const auto &u_img, const auto &gen_iter, const_pointer trans_u_img){
			// root ~~~~~~~~> u ---> g[u]
			//  ^     T[u]       g    |
			//  |                     |
			//  \---------------------/
			//        T[g[u]]^-
			//
			// Add T[u] * g * T[g(u)]^- as a generator.
			const_pointer cand;
			const bool nonOwning = u == root && u_img == root;
			if(nonOwning) {
				cand = *gen_iter;
			} else if(u == root) { // then T[u] == id
				const_pointer trans_u_img_inv = inverse_trans[trans.orbit().position(u_img)];
				auto gen_expr = perm_group::mult(**gen_iter, *trans_u_img_inv);
				cand = this->get_allocator().copy(gen_expr);
			} else if(u_img == root) { // then T[g(u)]^- == id
				const_pointer trans_u = trans.from_element_as_ptr(u);
				auto gen_expr = perm_group::mult(*trans_u, **gen_iter);
				cand = this->get_allocator().copy(gen_expr);
			} else {
				const_pointer trans_u = trans.from_element_as_ptr(u);
				const_pointer trans_u_img_inv = inverse_trans[trans.orbit().position(u_img)];
				auto gen_expr = perm_group::mult(perm_group::mult(*trans_u, **gen_iter), *trans_u_img_inv);
				cand = this->get_allocator().copy(gen_expr);
			}
			// check if we already have it
			const auto gens = this->generators();
			const bool isDup = dupChecker(gens.begin(), gens.end(), *cand);
			if(isDup) {
				if(!nonOwning)
					this->get_allocator().release(cand);
			} else {
				if(nonOwning)
					cand = this->get_allocator().copy(**gen_iter);
				gen_set.push_back(cand);
				next(gen_set.begin(), gen_set.end() - 1, gen_set.end());
			}
		};
		trans.update(first, lastOld, lastNew, onNewElement, onDupElement);
	}
public:

	template<typename Perm>
	const_pointer sift_factor(const Perm &p) const {
		const auto root = trans.get_root();
		const auto img = perm_group::get(p, root);
		const auto &o = trans.orbit();
		if(!o.isInOrbit(img)) return nullptr;
		// p sends root -> img
		// find the inverse of the representatives, i.e., a perm with img -> root
		// compose to fix root
		const auto pos = o.position(img);
		auto t_img_inv = inverse_trans[pos];
		return t_img_inv;
	}

	template<typename Perm>
	pointer sift(const Perm &p) const {
		const_pointer factor = sift_factor(p);
		if(!factor) return nullptr;
		auto comp = perm_group::mult(p, *factor);
		return get_allocator().copy(comp);
	}
public:

	const Transversal &transversal() const {
		return trans;
	}

	const_pointer inverse_transversal(typename perm::value_type pos) const {
		return inverse_trans[pos];
	}
private:
	Transversal trans;
	Store inverse_trans;
	Store gen_set;
public: // TODO: stop the haxing
	DupChecker dupChecker;
};

} // namespace perm_group

#endif /* PERM_GROUP_GROUP_SCHREIER_STABILIZER_HPP */