#ifndef _TELL_SOURCE_SRC_STACKTRACE_TELL_STACKTRACE_TO_CLEANED_STRING_HPP
#define _TELL_SOURCE_SRC_STACKTRACE_TELL_STACKTRACE_TO_CLEANED_STRING_HPP

#include <string>

#include <boost/algorithm/cxx11/none_of.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/stacktrace.hpp>

namespace tell { namespace except { namespace detail {

	/// \brief Generate a string for the specified stacktrace, stripped of any frames from tell's throw_with_locn_and_stacktrace()
	template <typename Rng>
	inline ::std::string to_string_stripped_by_prefixes(const ::boost::stacktrace::stacktrace  &prm_stacktrace,    ///< The stacktrace to describe
	                                                    Rng                                   &&prm_strip_prefixes ///< A range of prefixes to specify the stacktrace entries to be filtered out
	                                                    ) {
		// Lambda closure for whether the argument's `.name()` reveals it comes from tell's throw_with_locn_and_stacktrace()
		const auto is_outside_throw_context = [&] (const auto &x) {
			return ::boost::algorithm::none_of(
				prm_strip_prefixes,
				[&] (auto &&y) {
					return ::boost::algorithm::starts_with( x.name(), y );
				}
			);
		};
		// Loop over the acceptable frames and append a description of each to a string, then return the string
		::std::string result;
		return ::boost::algorithm::join(
			prm_stacktrace
				| ::boost::adaptors::filtered( is_outside_throw_context )
				| ::boost::adaptors::indexed()
				// | ::boost::adaptors::transformed( [] (const boost::range::index_value<const boost::stacktrace::frame &, long> &indexed_frame) {
				| ::boost::adaptors::transformed( [] (const auto                                                              &indexed_frame) {
					return ( ::boost::format( "%4d" ) % indexed_frame.index() ).str()
						+ "  "
						+ to_string( indexed_frame.value() )
						+ "\n";
				} ),
			""
		);
	}

} // namespace detail
} // namespace except
} // namespace tell

#endif // _TELL_SOURCE_SRC_STACKTRACE_TELL_STACKTRACE_TO_CLEANED_STRING_HPP
