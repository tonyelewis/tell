#ifndef TELL_THROW_HPP
#define TELL_THROW_HPP

#include <boost/stacktrace.hpp>

#include "tell/detail/types.hpp"

namespace tell { namespace except { namespace detail {

	/// \brief Use Boost exception to decorate the specified argument with the throw location and stacktrace
	///
	/// Don't use this directly - call it via the TELL_THROW() macro
	///
	/// It's better to have the call to stacktrace() in the body of this function rather
	/// than in the call because otherwise the stack on clang+addr2line can miss out a
	/// decent location for the call.
	template <typename Ex>
	void throw_with_locn_and_stacktrace(Ex                                    &&prm_exception, ///< The thing to be decorated and then thrown
	                                    const throw_function_value_t           &prm_function,  ///< The name of the function containing the code that wants to throw
	                                    const throw_file_value_t               &prm_file,      ///< The name of the source file containing the code that wants to throw
	                                    const throw_line_value_t               &prm_line       ///< The number of the source line containing the code that wants to throw
	                                    )
	{
		throw (
			::boost::enable_error_info( std::forward<Ex>( prm_exception ) )
				<< ::boost::throw_function               ( prm_function                      )
				<< ::boost::throw_file                   ( prm_file                          )
				<< ::boost::throw_line                   ( prm_line                          )
				<< boost_exception_stacktrace_error_info ( ::boost::stacktrace::stacktrace() )
		);
	}

} } } // namespace tell::except::detail

/// \brief Use Boost exception to decorate the specified argument with the throw location and stacktrace
#define TELL_THROW(x) ::tell::except::detail::throw_with_locn_and_stacktrace( ((x)), __FUNCTION__, __FILE__, __LINE__ )

#endif // TELL_THROW_HPP
