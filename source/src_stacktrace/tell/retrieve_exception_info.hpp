#ifndef _TELL_SOURCE_SRC_STACKTRACE_TELL_RETRIEVE_EXCEPTION_INFO_HPP
#define _TELL_SOURCE_SRC_STACKTRACE_TELL_RETRIEVE_EXCEPTION_INFO_HPP

#include <string>
#include <type_traits>

#include <boost/exception/get_error_info.hpp>
#include <boost/optional.hpp>

#include "tell/detail/types.hpp"
#include "tell/stacktrace_to_cleaned_string.hpp"

namespace tell { namespace except {

	namespace detail {

		/// Type trait to remove the const, volatile, reference from a type
		///
		/// This is similar to decay but it doesn't decay arrays or functions to pointers
		///
		/// This is a local version of the type trait coming C++20
		///
		/// TODO: Come C++20, replace this with the std:: version
		template <typename T>
		struct remove_cvref {
			using type = ::std::remove_cv_t< ::std::remove_reference_t<T> >;
		};

		/// Type alias to remove the const, volatile, reference from a type
		///
		/// This is similar to decay_t but it doesn't decay arrays or functions to pointers
		///
		/// This is a local version of the type alias coming in C++20
		///
		/// TODO: Come C++20, replace this with the std:: version
		template <typename T >
		using remove_cvref_t = typename remove_cvref<T>::type;

		/// \brief Return a std::true_type value if Base is a bse of the value's static type, or a std::false_type value otherwise
		template <typename Base, typename Val>
		constexpr auto tag_value_for_is_base_of_arg(Val &&/* prm_value */ ///< The value whose static type is to be checked for inheriting from Base
		                                            ) -> typename ::std::is_base_of<Base, remove_cvref_t<Val>>::type {
			return {};
		}

		/// \brief Tag-dispatch overload for getting the what() of a type that isn't a std::exception, so just return none
		template <typename Ex>
		inline ::boost::optional<::std::string> get_what_of_std_exception_impl(const ::std::false_type &/* inherits_from_std_exception_tag */, ///< Type to indicate that std::exception *is not* a base of Ex
		                                                                       const Ex                &prm_value                              ///< The object of some type not derived from ::std::exception
		                                                                       ) {
			const auto std_except_ptr = dynamic_cast<const ::std::exception *>( &prm_value );
			return ( std_except_ptr != nullptr )
				? ::boost::make_optional( ::std::string{ std_except_ptr->what() } )
				: ::boost::none;
		}

		/// \brief Tag-dispatch overload for getting the what() of a std::exception, so do that 
		inline ::boost::optional<::std::string> get_what_of_std_exception_impl(const ::std::true_type &/* inherits_from_std_exception_tag */, ///< Type to indicate that std::exception *is* a base of Ex
		                                                                       const std::exception   &prm_value                              ///< The object of some type derived from ::std::exception
		                                                                       ) {
			return ::std::string{ prm_value.what() };
		}

		/// \brief Return the what() from the specified value if it's of a type derived from std::exception, or none otherwise
		///
		/// More specifically if std::exception is a base of the argument's static type, this calls what() directly
		/// otherwise, it attempts to dynamic_cast to a std::exception and then call what() on the result if successful
		template <typename Ex>
		inline ::boost::optional<::std::string> get_what_of_std_exception(const Ex &prm_value ///< The value to query
		                                                                  ) {
			return get_what_of_std_exception_impl( tag_value_for_is_base_of_arg< ::std::exception >( prm_value ), prm_value );
		}

	} // namespace detail

	/// \brief Generate a string describing the specified boost::exception, retrieving info added by TELL_THROW()
	template <typename Ex>
	inline std::string retrieve_exception_info(const Ex &prm_exception ///< The boost::exception, hopefully thrown via TELL_THROW
	                                           ) {
		static_assert( ::std::is_base_of<::boost::exception, detail::remove_cvref_t<Ex>>::value,
			"tell can only retrieve_exception_info() when passed with a static type derived from boost::exception (or boost::exception itself)" );

		const auto &file_value_ptr     = ::boost::get_error_info< ::boost::throw_file                           >( prm_exception );
		const auto &line_value_ptr     = ::boost::get_error_info< ::boost::throw_line                           >( prm_exception );
		const auto &function_value_ptr = ::boost::get_error_info< ::boost::throw_function                       >( prm_exception );
		const auto &stacktrace_ptr     = ::boost::get_error_info< detail::boost_exception_stacktrace_error_info >( prm_exception );
		const auto &what_msg           = detail::get_what_of_std_exception( prm_exception );
		const auto &dynamic_type_name  = ::boost::core::demangle( typeid( prm_exception ).name() );

		const auto prefixes_to_remove  = { "void tell::except::detail::throw_with_locn_and_stacktrace" };

		return "Retrieving "
			+ dynamic_type_name
			+ " that had been thrown"
			+ (
				( function_value_ptr != nullptr )
				? ( " in '" + ::std::string{ *function_value_ptr } + "'" )
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
				? "\nStacktrace:\n" + detail::to_string_stripped_by_prefixes(
					*stacktrace_ptr,
					prefixes_to_remove
				)
				: ""
			);
	}

	/// \brief Generate a string describing the specified boost::exception, retrieving info added by TELL_THROW()
	///        including context information about where the information is being retrieved
	///
	/// Don't use this function directly, use the macro TELL_RETRIEVE_EXCEPTION_INFO()
	template <typename Ex>
	inline std::string retrieve_exception_info(const Ex                             &prm_exception, ///< The boost::exception, hopefully thrown via TELL_THROW
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

} // namespace except
} // namespace tell

/// \brief Generate a string describing the specified boost::exception, retrieving info added by TELL_THROW()
///        including context information about where the information is being retrieved
#define TELL_RETRIEVE_EXCEPTION_INFO(x) ::tell::except::retrieve_exception_info( ((x)), BOOST_THROW_EXCEPTION_CURRENT_FUNCTION, __FILE__, __LINE__ )

#endif // _TELL_SOURCE_SRC_STACKTRACE_TELL_RETRIEVE_EXCEPTION_INFO_HPP
