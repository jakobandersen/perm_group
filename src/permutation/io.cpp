#include <perm_group/permutation/io.hpp>

#include <boost/lexical_cast.hpp>

#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_rule.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_uint.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

namespace perm_group {
namespace {
namespace qi = boost::spirit::qi;

template<typename Iter>
struct cycles_grammar : qi::grammar<Iter, std::vector<std::vector<std::size_t> >(), qi::space_type> {

	cycles_grammar() : cycles_grammar::base_type(start) {
		start %= +cycle;
		cycle %= qi::lit('(') > +qi::uint_ >> qi::lit(')');
	}
private:
	qi::rule<Iter, std::vector<std::vector<std::size_t> >(), qi::space_type> start;
	qi::rule<Iter, std::vector<std::size_t>(), qi::space_type> cycle;
};

cycles_grammar<boost::spirit::istream_iterator> cycles_grammar_istream;

template<typename Expr, typename Skipper, typename Attr>
bool parse(std::istream &s, const Expr &expr, const Skipper &skipper, Attr &attr) {

	struct FlagsHandler {

		FlagsHandler(std::istream &s) : s(s), flags(s.flags()) {
			s.unsetf(std::ios::skipws);
		}

		~FlagsHandler() {
			s.flags(flags);
		}
	private:
		std::istream &s;
		std::ios::fmtflags flags;
	} flagsHandler(s);
	boost::spirit::istream_iterator iter(s), iter_end;
	return qi::phrase_parse(iter, iter_end, expr, skipper, attr);
}

} // namespace

void check_element(std::size_t i, std::size_t n) {
	if(i < n) return;
	std::string msg = "Element " + boost::lexical_cast<std::string>(i);
	msg += " out of range [0;";
	msg += boost::lexical_cast<std::string>(n);
	msg += "[.";
	throw io_error(msg);
}

void check_set_hit(std::size_t i, std::vector<bool> &hit) {
	assert(i < hit.size());
	if(hit[i]) {
		std::string msg = "Element " + boost::lexical_cast<std::string>(i);
		msg += " already used.";
		throw io_error(msg);
	}
	hit[i] = true;
}

std::vector<std::vector<std::size_t> > read_permutation_cycles(std::istream &s) {
	std::vector<std::vector<std::size_t> > cycles;
	bool res = parse(s, cycles_grammar_istream, qi::space, cycles);
	if(!res) throw io_error("Could not parse cycle notation for permutation.");
	return cycles;
}

} // namespace perm_group