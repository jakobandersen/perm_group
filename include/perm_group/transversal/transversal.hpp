#ifndef PERM_GROUP_TRANSVERSAL_TRANSVERSAL_HPP
#define PERM_GROUP_TRANSVERSAL_TRANSVERSAL_HPP

#include <perm_group/allocator/allocator.hpp>

#include <boost/concept_check.hpp>

namespace perm_group {

// rst: .. todo:: Actually write the documentation for this file.

template<typename Trans>
struct TransversalConcept {
	using allocator = typename Trans::allocator;
	BOOST_CONCEPT_ASSERT((Allocator<allocator>));
	using orbit_type = typename Trans::orbit_type;

	using perm = typename allocator::perm;
	using value_type = typename perm::value_type;

	BOOST_CONCEPT_USAGE(TransversalConcept) {
		allocator alloc = cTrans.get_allocator();
		(void) alloc;
		const orbit_type &o = cTrans.orbit();
		value_type u = cTrans.get_root();
		(void) u;
	}
private:
	const Trans cTrans;
};
// TransversalConcept
// orbit(t)

} // namespace perm_group

#endif /* PERM_GROUP_TRANSVERSAL_TRANSVERSAL_HPP */