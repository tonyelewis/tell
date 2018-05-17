#ifndef TELL_A
#define TELL_A

#include "tell/detail/types.hpp"

#include <string>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/exception/get_error_info.hpp>
#include <boost/format.hpp>
#include <boost/optional.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <boost/stacktrace.hpp>

namespace tell { namespace except {

	namespace detail {

		/// \brief Tag-dispatch overload for getting the what() of a type that isn't a std::exception, so just return none
		template <typename Ex>
		::boost::optional<::std::string> get_what_of_std_exception_impl(const ::std::false_type &, ///< Type to indicate that std::exception *is not* a base of Ex
		                                                                const Ex &
		                                                                ) {
			return ::boost::none;
		}

		/// \brief Tag-dispatch overload for getting the what() of a std::exception, so do that 
		::boost::optional<::std::string> get_what_of_std_exception_impl(const ::std::true_type &, ///< Type to indicate that std::exception *is* a base of Ex
		                                                                const std::exception &prm_value ///< The object of some type derived from ::std::exception
		                                                                ) {
			return ::std::string{ prm_value.what() };
		}

		/// \brief Return the what() from the specified value if it's of a type derived from std::exception, or none otherwise
		template <typename Ex>
		::boost::optional<::std::string> get_what_of_std_exception(const Ex &prm_value ///< The value to query
		                                                           ) {
			return get_what_of_std_exception_impl( ::std::is_base_of<::std::exception, std::decay_t<Ex>>{}, prm_value );
		}

		/// \brief Generate a string for the specified stacktrace, stripped of any frames from tell's throw_with_locn_and_stacktrace()
		::std::string to_string_cleaned_of_throw_context(const ::boost::stacktrace::stacktrace &prm_stacktrace ///< The stacktrace to describe
		                                                 ) {
			// Lambda closure for whether the argument's `.name()` reveals it comes from tell's throw_with_locn_and_stacktrace()
			const auto is_outside_throw_context = [] (const auto &x) {
				return ! ::boost::algorithm::starts_with(
					x.name(),
					"void tell::except::detail::throw_with_locn_and_stacktrace"
				);
			};

			// Loop over the acceptable frames and append a description of each to a string, then return the string
			std::string result;
			for (const auto &indexed_frame : prm_stacktrace | ::boost::adaptors::filtered( is_outside_throw_context ) | ::boost::adaptors::indexed() ) {
				result += ( ::boost::format( "%4d" ) % indexed_frame.index() ).str()
					+ "  "
					+ to_string( indexed_frame.value() )
					+ "\n";
			}
			return result;
		}

	} // namespace detail

	/// \brief Generate a string describing the specified boost::exception, retrieving info added by TELL_THROW()
	std::string retrieve_exception_info(const boost::exception &prm_exception ///< The boost::exception, hopefully thrown via TELL_THROW
	                                    ) {
		const auto &file_value_ptr     = ::boost::get_error_info< ::boost::throw_file                           >( prm_exception );
		const auto &line_value_ptr     = ::boost::get_error_info< ::boost::throw_line                           >( prm_exception );
		const auto &function_value_ptr = ::boost::get_error_info< ::boost::throw_function                       >( prm_exception );
		const auto &stacktrace_ptr     = ::boost::get_error_info< detail::boost_exception_stacktrace_error_info >( prm_exception );
		const auto &what_msg           = detail::get_what_of_std_exception( prm_exception );

		const auto &dynamic_type_name  = ::boost::core::demangle( typeid( prm_exception ).name() );
		return "Retrieving "
			+ dynamic_type_name
			+ " that had been thrown"
			+ (
				( function_value_ptr != nullptr )
				? ( " in '" + ::std::string   { *function_value_ptr } + "'" )
				: ""
			)
			+ (
				( file_value_ptr != nullptr && line_value_ptr != nullptr )
				? " at "
					+ ::std::string   { *file_value_ptr     }
					+ ":"
					+ ::std::to_string( *line_value_ptr     )
				: ""
			)
			+ (
				what_msg
				? ( " with message '" + *what_msg + "'" )
				: ""
			)
			+ (
				( stacktrace_ptr != nullptr )
				? "\nStacktrace:\n" + detail::to_string_cleaned_of_throw_context( *stacktrace_ptr )
				: ""
			);
	}


	/// \brief Generate a string describing the specified boost::exception, retrieving info added by TELL_THROW()
	///        including context information about where the information is being retrieved
	///
	/// Don't use this function directly, use the macro TELL_RETRIEVE_EXCEPTION_INFO()
	std::string retrieve_exception_info(const boost::exception               &prm_exception, ///< The boost::exception, hopefully thrown via TELL_THROW
	                                    const detail::throw_function_value_t &prm_function,  ///< The name of the function containing the code that wants to retrieve this information
	                                    const detail::throw_file_value_t     &prm_file,      ///< The name of the source file containing the code that wants to retrieve this information
	                                    const detail::throw_line_value_t     &prm_line       ///< The number of the source line containing the code that wants to retrieve this information
	                                    ) {
		return "In '"
			+ ::std::string   ( prm_function )
			+ "' at "
			+ ::std::string   ( prm_file     )
			+ ":"
			+ ::std::to_string( prm_line     )
			+ " : "
			+ retrieve_exception_info( prm_exception );
	}

}} // namespace tell::except

/// \brief Generate a string describing the specified boost::exception, retrieving info added by TELL_THROW()
///        including context information about where the information is being retrieved
#define TELL_RETRIEVE_EXCEPTION_INFO(x) ::tell::except::retrieve_exception_info( ((x)), __FUNCTION__, __FILE__, __LINE__ )

#endif // TELL_A
