#ifndef TELL_TYPES
#define TELL_TYPES

#include <boost/exception/info.hpp>
#include <boost/stacktrace/stacktrace_fwd.hpp>

namespace tell { namespace except { namespace detail {

	/// \brief Type alias for the type that Boost Exception's boost::throw_function uses to store the name of the function containing the code that wants to throw
	using throw_function_value_t = typename ::boost::throw_function::value_type;

	/// \brief Type alias for the type that Boost Exception's boost::throw_file uses to store the name of the source file containing the code that wants to throw
	using throw_file_value_t     = typename ::boost::throw_file::value_type;

	/// \brief Type alias for the type that Boost Exception's boost::throw_line uses to store the number of the source line containing the code that wants to throw
	using throw_line_value_t     = typename ::boost::throw_line::value_type;

	/// \brief A tag to use in a boost::error_info as a key that indicates a stacktrace value
	struct boost_exception_stacktrace_tag final {
		boost_exception_stacktrace_tag() = delete;
		~boost_exception_stacktrace_tag() = delete;
		boost_exception_stacktrace_tag(const boost_exception_stacktrace_tag &) = delete;
		boost_exception_stacktrace_tag & operator=(const boost_exception_stacktrace_tag &) = delete;
	};

	/// \brief An error_info that stores a stacktrace under the boost_exception_stacktrace_tag tag
	using boost_exception_stacktrace_error_info = ::boost::error_info<boost_exception_stacktrace_tag, ::boost::stacktrace::stacktrace>;

} } } // namespace tell::except::detail

#endif // TELL_TYPES