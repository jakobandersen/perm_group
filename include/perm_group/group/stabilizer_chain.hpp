#ifndef PERM_GROUP_GROUP_STABILIZER_CHAIN_HPP
#define PERM_GROUP_GROUP_STABILIZER_CHAIN_HPP

#include <perm_group/group/generated.hpp>
#include <perm_group/group/schreier_stabilizer.hpp>
#include <perm_group/permutation/word.hpp>

#include <vector>

//#define PERM_GROUP_STABILIZER_CHAIN_DEBUG

#ifdef PERM_GROUP_STABILIZER_CHAIN_DEBUG
#include <perm_group/permutation/io.hpp>
#include <iostream>
#endif

namespace perm_group {

// rst: .. todo:: Actually write the documentation for this file.

struct base_point_first_moved {

	template<typename Perm>
	typename permutation_traits<Perm>::value_type operator()(const Perm &p) const {
		for(typename permutation_traits<Perm>::value_type i = 0; i != perm_group::degree(p); ++i) {
			if(perm_group::get(p, i) != i)
				return i;
		}
		return 0;
	}
};

template<typename Transversal, typename BasePointProvider = base_point_first_moved>
struct stabilizer_chain {
public: // GroupConcept
	using allocator = typename Transversal::allocator;
	using perm = typename allocator::perm;
	using pointer = typename allocator::pointer;
	using const_pointer = typename allocator::const_pointer;
public: // StabilizerConcept
	using is_accurate = std::true_type;
public:

	struct DupCheckerIsMember {

		DupCheckerIsMember(const stabilizer_chain *owner) : owner(owner) { }

		template<typename Iter>
		bool operator()(Iter first, Iter last, const perm &p) const {
			return owner->is_member(p);
		}
	public:
		const stabilizer_chain *owner;
	};
	using stabilizer = schreier_stabilizer<Transversal, DupCheckerIsMember>;
	using value_type = typename permutation_traits<perm>::value_type;
public:

	stabilizer_chain(std::size_t fixed, const allocator &alloc)
	: stabilizer_chain(fixed, alloc, BasePointProvider()) { }

	stabilizer_chain(std::size_t fixed, const allocator &alloc, BasePointProvider bpp)
	: stab(fixed, alloc, DupCheckerIsMember(this)), bpp(bpp) { }

	stabilizer_chain(stabilizer_chain &&other) : stabilizer_chain(other.fixed_element(), other.get_allocator(), other.bpp) {
		using std::swap;
		swap(stab, other.stab);
		swap(next, other.next);
		stab.dupChecker.owner = this;
		other.stab.dupChecker.owner = &other;
	}

	stabilizer_chain &operator=(stabilizer_chain &&other) {
		using std::swap;
		swap(bpp, other.bpp);
		swap(stab, other.stab);
		swap(next, other.next);
		stab.dupChecker.owner = this;
		other.stab.dupChecker.owner = &other;
	}
public: // StabilizerConcept

	std::size_t fixed_element() const {
		return stab.fixed_element();
	}

	template<typename Iter>
	void add_generators(Iter first, Iter lastOld, Iter lastNew) {
		add_generators(first, lastOld, lastNew, [](auto&&... args) {
		});
	}

	template<typename Iter, typename Next>
	void add_generators(Iter first, Iter lastOld, Iter lastNew, Next next) {
#ifdef PERM_GROUP_STABILIZER_CHAIN_DEBUG
		std::cout << "add_generators(" << this << "):" << std::endl;
		for(auto iter = lastOld; iter != lastNew; ++iter)
			write_permutation_cycles(std::cout << "\t", **iter) << std::endl;
#endif
		stab.add_generators(first, lastOld, lastNew, [&](auto firstInner, auto lastOldInner, auto lastNewInner) {
			// first complete the addition to the chain
			if(!this->next) { // add another level
				auto toFix = bpp(**lastOldInner);
				this->next.reset(new stabilizer_chain(toFix, this->get_allocator(), bpp));
			}
			this->next->add_generators(firstInner, lastOldInner, lastNewInner);
			// and then tell the user about it
			next(firstInner, lastOldInner, lastNewInner);
		});
	}
public:

	bool is_member(const perm &p) const {
#ifdef PERM_GROUP_STABILIZER_CHAIN_DEBUG
		write_permutation_cycles(std::cout << "is_member(" << this << ", ", p) << "): ";
		std::cout << "next = " << next.get() << std::endl;
		if(next) std::cout << "\trecurse" << std::endl;
		else std::cout << "\tif identity" << std::endl;
#endif
		if(!next) {
			assert(generators().size() == 1);
			return p == *generators().begin();
		} else {
			return next->is_member_of_parent(p);
		}
	}

	bool is_member_of_parent(const perm &p) const {
#ifdef PERM_GROUP_STABILIZER_CHAIN_DEBUG
		write_permutation_cycles(std::cout << "is_member_of_parent(" << this << ", ", p) << "):" << std::endl;
#endif
		word.clear();
		const auto rest = perm_group::mult(p, word); // note, this changes when word changes

		// do the first one specially as we sift p and not a compound permutation
		const auto *stab_chain = this;
		do {
			const auto fixed = stab_chain->fixed_element();
			if(perm_group::get(p, fixed) == fixed) {
				stab_chain = stab_chain->get_next();
			} else {
				const_pointer factor = stab_chain->stab.sift_factor(p);
#ifdef PERM_GROUP_STABILIZER_CHAIN_DEBUG
				std::cout << "\tfactor = ";
				if(!factor) std::cout << "nullptr" << std::endl;
				else write_permutation_cycles(std::cout, *factor) << std::endl;
#endif
				if(!factor) return false;
				word.push_back(factor);
#ifdef PERM_GROUP_STABILIZER_CHAIN_DEBUG
				write_permutation_cycles(std::cout << "\trest = ", rest) << std::endl;
#endif
				stab_chain = stab_chain->get_next();
				break;
			}
		} while(stab_chain);

		// word now contains exactly 1 permutation, and we are already at the next stabilizer
		for(; stab_chain; stab_chain = stab_chain->get_next()) {
			if(perm_group::get(rest, stab_chain->fixed_element()) == stab_chain->fixed_element())
				continue; // it's definitely in this stabilizer, no point in composing with the identity perm
			const_pointer factor = stab_chain->stab.sift_factor(rest);
#ifdef PERM_GROUP_STABILIZER_CHAIN_DEBUG
			std::cout << "\tfactor = ";
			if(!factor) std::cout << "nullptr" << std::endl;
			else write_permutation_cycles(std::cout, *factor) << std::endl;
#endif
			if(!factor) return false;
			word.push_back(factor);
#ifdef PERM_GROUP_STABILIZER_CHAIN_DEBUG
			write_permutation_cycles(std::cout << "\trest = ", rest) << std::endl;
#endif
		}
#ifdef PERM_GROUP_STABILIZER_CHAIN_DEBUG
		bool debug_is_member = true;
		for(value_type i = 0; i != degree(); ++i) {
			if(perm_group::get(rest, i) != i)
				debug_is_member = false;
		}
		std::cout << "\tis_member = " << std::boolalpha << debug_is_member << std::endl;
#endif
		// if it's the identity we have it
		if(word.empty()) { // optimization: no point in involving the word if it's id
			return p == *generators().begin();
		} else {
			for(value_type i = 0; i != degree(); ++i) {
				if(perm_group::get(rest, i) != i)
					return false;
			}
		}
		return true;
	}

	const stabilizer_chain *get_next() const {
		return next.get();
	}

	const Transversal &transversal() const {
		return stab.transversal();
	}
public: // GroupConcept

	decltype(auto) degree() const {
		return stab.degree();
	}

	decltype(auto) generators() const {
		return stab.generators();
	}

	decltype(auto) generator_ptrs() const {
		return stab.generator_ptrs();
	}

	decltype(auto) get_allocator() const {
		return stab.get_allocator();
	}
private:
	BasePointProvider bpp;
	stabilizer stab;
	std::unique_ptr<stabilizer_chain> next;
private:
	mutable permutation_word<const_pointer> word;
};

} // namespace perm_group

#endif /* PERM_GROUP_GROUP_STABILIZER_CHAIN_HPP */