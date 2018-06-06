#ifndef _TELL_SOURCE_SRC_STACKTRACE_TELL_BOOST_ASSERT_HPP
#define _TELL_SOURCE_SRC_STACKTRACE_TELL_BOOST_ASSERT_HPP

#include <iostream>

#include <boost/stacktrace.hpp>

#include "tell/detail/types.hpp"
#include "tell/stacktrace_to_cleaned_string.hpp"

using namespace ::std::literals::string_literals;

namespace boost {

	/// \brief Handler for BOOST_ASSERT_MSG() failures
	///
	/// This outputs a string describing the assertion failure with a stacktrace context
	/// and then call abort() (which may result in a core dump, depending on system settings)
	inline void assertion_failed_msg(char const * prm_expr,     ///< The assertion expression that has failed
	                                 char const * prm_msg,      ///< The message associated with the assertion or nullptr if none
	                                 char const * prm_function, ///< The name of the function containing the assertion
	                                 char const * prm_file,     ///< The name of the file containing the assertion
	                                 int64_t      prm_line      ///< The line number on which the assertion appears
	                                 ) {
		const auto prefixes_to_remove = { "boost::assertion_failed" };
		// Output information about the failure, with frequent flushing
		::std::cerr
			<< "[ASSERTION ERROR] Failed assertion '" << ::std::flush
			<< prm_expr                               << ::std::flush
			<< (
				( prm_msg != nullptr )
				? ( "' with message '"s + prm_msg )
				: ""s
			)
			<< "' at "                                << ::std::flush
			<< prm_file
			<< ":"                                    << ::std::flush
			<< prm_line
			<< " in '"                                << ::std::flush
			<< prm_function
			<< "'\nStacktrace:\n"                     << ::std::flush
			<< ::tell::except::detail::to_string_stripped_by_prefixes(
				::boost::stacktrace::stacktrace(),
				prefixes_to_remove
			) << ::std::flush;
			abort();
	}

	/// \brief Handler for BOOST_ASSERT() failures
	///
	/// This is implemented through a call to BOOST_ASSERT_MSG() with nullptr for the message.
	///
	/// This outputs a string describing the assertion failure with a stacktrace context
	/// and then call abort() (which may result in a core dump, depending on system settings)
	inline void assertion_failed(char const * prm_expr,     ///< The assertion expression that has failed
	                             char const * prm_function, ///< The name of the function containing the assertion
	                             char const * prm_file,     ///< The name of the file containing the assertion
	                             int64_t      prm_line      ///< The line number on which the assertion appears
	                             ) {
		::boost::assertion_failed_msg(
			prm_expr,
			nullptr,
			prm_function,
			prm_file,
			prm_line
		);
	}
} // namespace boost

#endif // _TELL_SOURCE_SRC_STACKTRACE_TELL_BOOST_ASSERT_HPP
