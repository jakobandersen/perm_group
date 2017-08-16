#ifndef PERM_GROUP_TEST_SAGE_HPP
#define PERM_GROUP_TEST_SAGE_HPP

#include <perm_group/permutation/permutation.hpp>

#include <boost/process.hpp>
#include <boost/process/extend.hpp>

namespace bp = boost::process;

namespace perm_group {

struct Sage {

	struct ChildOnExecHandler : bp::extend::handler {

		ChildOnExecHandler(Sage &sage) : sage(sage) { }

		template<typename Executor>
		void on_exec_error(Executor&, const std::error_code&) const {
			::close(sage.stdin.pipe().native_sink());
		}
	private:
		Sage &sage;
	};
public:

	Sage(std::size_t verbose) : verbose(verbose),
	child("sage", "-python", bp::shell,
	bp::std_in < stdin/*, bp::std_out > stdout*/,
	ChildOnExecHandler(*this)
	) {
		stdin << "from sage.all import *\n";
	}

	~Sage() {
		stdin.flush();
		stdin.pipe().close();
		child.wait();
		if(child.exit_code() != 0)
			throw std::runtime_error("Non-zero exit code from Sage: " + std::to_string(child.exit_code()));
	}
public:

	struct Printer {

		Printer(std::size_t verbose, bp::opstream &s, std::size_t &lineNo) : verbose(verbose), s(s), lineNo(lineNo) { }

		Printer(Printer &&p) : enabled(p.enabled), verbose(p.verbose), s(p.s), ss(std::move(p.ss)), lineNo(p.lineNo) {
			p.enabled = false;
		}

		~Printer() {
			if(!enabled) return;
			std::string str = ss.str();
			s << str;
			if(verbose <= 1) return;
			bool startOfLine = true;
			std::ostream &s = std::cout;
			for(auto c : str) {
				if(startOfLine) s << "Sage:" << lineNo << ": ";
				startOfLine = false;
				if(c == '\n') {
					startOfLine = true;
					++lineNo;
				}
				s << c;
			}
		}

		template<typename T>
		friend Printer &operator<<(Printer &p, T &&t) {
			p.ss << std::forward<T>(t);
			return p;
		}

		template<typename T>
		friend Printer operator<<(Printer &&p, T &&t) {
			p.ss << std::forward<T>(t);
			return std::move(p);
		}

		std::ostream &stream() {
			return ss;
		}
	private:
		bool enabled = true;
		std::size_t verbose;
		bp::opstream &s;
		std::stringstream ss;
		std::size_t &lineNo;
	};

	Printer print() {
		return Printer(verbose, stdin, lineNo);
	}

	void line(const std::string &s) {
		print() << s << '\n';
	}

	void print(const std::string &s) {
		print() << "print('Sage: " << s << "')\n";
	}

	void printTest(const std::string &s) {
		if(verbose == 0) return;
		print()
				<< "print('Sage: " << s << "')\n"
				<< "print('=' * 80)\n";
	}

	void printRound(std::size_t round) {
		if(verbose == 0) return;
		print()
				<< "print('Sage: Round %d' % " << round << ")\n"
				<< "print('#' * 80)\n";
	}

	void printSubRound(std::size_t round) {
		if(verbose == 0) return;
		print()
				<< "print('Sage: Sub-round %d' % " << round << ")\n"
				<< "print('=' * 80)\n";
	}

	template<typename C>
	void checkOrbit(const std::string &gVar, std::size_t i, const C &orbit) {
		Printer s = print();
		s << "o = set(" << gVar << ".orbit(" << i << "))\n";
		s << "oCheck = set([";
		std::copy(orbit.begin(), orbit.end(), std::ostream_iterator<int>(s.stream(), ", "));
		s << "])\n";
		s << "if o != oCheck:\n";
		s << "\tprint('Sage: %s' % str(o))\n";
		s << "\tprint('Us:   %s' % str(oCheck))\n";
		s << "\tassert False\n";
	}

	void checkSubgroup(const std::string &gSub, const std::string &gSuper, const std::string &onError = "lambda x,y: None") {
		if(checkSubgroupFirst) {
			checkSubgroupFirst = false;
			Printer s = print();
			s << "def checkSubgroup(sub, super, onError):\n"
					<< "\tif not sub.is_subgroup(super):\n"
					<< "\t\tonError(sub, super)\n"
					<< "\t\tprint('Subgroup:   %s' % str(sub))\n"
					<< "\t\tprint('Supergroup: %s' % str(super))\n"
					<< "\t\tassert False\n";
		}
		Printer s = print();
		s << "checkSubgroup(" << gSub << ", " << gSuper << ", " << onError << ")\n";
	}
public:

	template<typename GenPtrIter>
	void loadGroup(GenPtrIter first, GenPtrIter last, std::size_t degree, const std::string &var) {
		// typedefs to trigger erroneous types
		using PermPtr = typename std::iterator_traits<GenPtrIter>::value_type;
		using Perm = typename std::pointer_traits<PermPtr>::element_type;

		Printer s = print();
		s << var << " = PermutationGroup([\n";
		for(; first != last; ++first)
			s << "\t" << perm(**first, degree) << ",\n";
		s << "], domain=[";
		for(auto i = 0; i < degree; ++i) s << i << ", ";
		s << "])\n";
	}

	template<typename Group>
	void loadGroup(const Group &g, const std::string &var) {
		auto gens = generator_ptrs(g);
		loadGroup(gens.begin(), gens.end(), degree(g), var);
	}

	void makeStab(std::size_t c, const std::string &parent, const std::string &var) {
		writeStdin([&](auto &s) {
			s << var << " = " << parent << ".stabilizer(" << c << ")\n";
		});
	}
private:

	template<typename F>
	void writeStdin(F f) {
		std::stringstream s;
		f(s);
		if(verbose > 1) std::cout << "Sage Input:\n'''''''''''''\n" << s.str() << ".............\n";
		stdin << s.rdbuf();
	}
public:

	template<typename Perm>
	struct PermStreamer {

		PermStreamer(const Perm &p, std::size_t degree) : p(p), degree(degree) { }

		friend std::ostream &operator<<(std::ostream &s, const PermStreamer &ps) {
			s << "[" << perm_group::get(ps.p, 0);
			for(auto i = 1; i < ps.degree; ++i)
				s << ", " << perm_group::get(ps.p, i);
			s << "]";
			return s;
		}
	private:
		const Perm &p;
		std::size_t degree;
	};

	template<typename Perm>
	static PermStreamer<Perm> perm(const Perm &p, std::size_t degree) {
		return PermStreamer<Perm>(p, degree);
	}
private:
	std::size_t verbose;
	bp::opstream stdin;
	bp::ipstream stdout;
	bp::child child;
	std::size_t lineNo = 1;
private:
	bool checkSubgroupFirst = true;
};

} // namespace perm_group

#endif /* PERM_GROUP_TEST_SAGE_HPP */
