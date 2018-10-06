#include <perm_group/permutation/built_in.hpp>
#include <perm_group/permutation/io.hpp>

int main() {
	std::vector<int> p(42);
	perm_group::read_permutation_cycles("(0)", p);
	perm_group::write_permutation_cycles(std::cout << "Id: ", p) << std::endl;
}
