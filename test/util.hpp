#ifndef PERM_GROUP_UTIL_HPP
#define PERM_GROUP_UTIL_HPP

#include <perm_group/allocator/shared_ptr.hpp>
#include <perm_group/allocator/pooled.hpp>
#include <perm_group/allocator/raw_ptr.hpp>

#include <boost/program_options.hpp>

#include <iostream>
#include <random>

namespace pg = perm_group;
namespace po = boost::program_options;

template<typename T>
struct Wrapper {
	using type = T;
public:

	Wrapper() {
		maker = [](std::size_t n) {
			return type(n);
		};
	}

	Wrapper(std::function<type(std::size_t) > maker) : maker(maker) { }

	type operator()(std::size_t n) const {
		return maker(n);
	}
public:
	std::function<type(std::size_t) > maker;
};

template<typename perm_type, typename F>
void runForEachAllocator(F f) {
	f(Wrapper<pg::shared_ptr_allocator<perm_type> >());
	f(Wrapper<pg::raw_ptr_allocator<perm_type> >());
	f(Wrapper<pg::pooled_allocator<pg::raw_ptr_allocator<perm_type> > >{[](auto n) {
			pg::raw_ptr_allocator<perm_type> inner(n);
			return pg::pooled_allocator<pg::raw_ptr_allocator<perm_type> >(n, inner);
		}});
}

struct TestProgram {
	using size_type = std::size_t;
	using perm_type = std::vector<size_type>;
public:

	TestProgram(int argc, char **argv) {
		po::options_description optionsDesc("Options");
		optionsDesc.add_options()
				("help,h", "Print help message.")
				("seed,s", po::value<std::size_t>(&seed)->default_value(std::random_device()()),
				"Seed for the random number generator.")
				("rounds,p", po::value<std::size_t>(&rounds)->default_value(100), "Number of repetitions.")
				("sub-rounds,q", po::value<std::size_t>(&subRounds)->default_value(5), "Number of inner loop repetitions.")
				("degree,n", po::value<std::size_t>(&degree)->default_value(5), "Degree of the group to use.")
				("gensize,g", po::value<std::size_t>(&gensize)->default_value(2), "Size of generating set.")
				("verbose,v", po::value<std::size_t>(&verbose)->default_value(0), "Verbosity level.")
				;

		po::variables_map options;
		try {
			po::store(po::command_line_parser(argc, argv).options(optionsDesc).run(), options);
		} catch(const po::error &e) {
			std::cout << e.what() << '\n';
			std::exit(1);
		}
		po::notify(options);
		if(options.count("help")) {
			std::cout << optionsDesc << std::endl;
			std::exit(0);
		}
		std::cout << "seed = " << seed << std::endl;
		std::cout << "rounds = " << rounds << std::endl;
		std::cout << "sub-rounds = " << subRounds << std::endl;
		std::cout << "degree = " << degree << std::endl;
		std::cout << "gensize = " << gensize << std::endl;
	}

	perm_type makeRandomPermutation() {
		std::vector<std::size_t> p(degree);
		for(std::size_t i = 0; i != degree; ++i) p[i] = i;
		std::shuffle(p.begin(), p.end(), engine);
		return p;
	}

	template<typename Tester>
	void run(Tester &&tester) {
		tester.noAllocator();
		runForEachAllocator<perm_type>([&](auto maker) {
			tester.withAllocator(maker);
		});
	}
public:
	std::size_t seed, rounds, subRounds, degree, gensize;
	std::size_t verbose;
	std::mt19937 engine;
};

#endif /* PERM_GROUP_UTIL_HPP */