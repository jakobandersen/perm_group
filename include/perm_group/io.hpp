#ifndef PERM_GROUP_IO_HPP
#define PERM_GROUP_IO_HPP

#include <perm_group/config.hpp>

#include <exception>
#include <string>

namespace perm_group {

// rst: .. class:: io_error : std::exception
// rst:
// rst:		Exception class thrown from IO functions.

struct PERM_GROUP_DECL io_error : std::exception {

	io_error(std::string s) : s(s) { }

	virtual ~io_error() { }

	virtual const char *what() const noexcept {
		return s.c_str();
	}
private:
	std::string s;
};

} // namespace perm_group

#endif /* PERM_GROUP_IO_HPP */
