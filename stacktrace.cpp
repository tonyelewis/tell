#include <iostream>

#include "tell/stacktrace_to_cleaned_string.hpp"
#include "tell/boost_assert.hpp"
#include "tell/retrieve_exception_info.hpp"
#include "tell/tell_throw.hpp"

using namespace ::std::literals::string_literals;

using ::std::cerr;
using ::std::flush;

namespace my { namespace stuff {

	/// \brief An base class for all exceptions in this application
	class app_exception_base : public virtual ::boost::exception,
	                           public virtual ::std::exception {
	private:
		/// \brief The message for the exception
		::std::string message;

	public:
		/// \brief Ctor from the message as std::string
		explicit app_exception_base(::std::string prm_message ///< The message to store in the exception
		                            ) : message{ ::std::move( prm_message ) } {
		}

		/// \brief Ctor from the message as char *
		explicit app_exception_base(char const * const &prm_message ///< The message to store in the exception
		                            ) : message{ prm_message } {
		}

		/// \brief Final implementation of std::exception's virtual method to return a char * message
		const char * what() const noexcept final {
			return message.c_str();
		}

		~app_exception_base() noexcept override = default;
		app_exception_base(const app_exception_base &) = default;
		app_exception_base(app_exception_base &&) = default;
		app_exception_base & operator=(const app_exception_base &) = default;
		app_exception_base & operator=(app_exception_base &&) = default;
	};

	/// \brief An exception to indicate a precondition error
	class precondition_error : public app_exception_base {
	public:
		/// \brief Ctor from the message as std::string
		explicit precondition_error(::std::string prm_message ///< The message to store in the exception
		                            ) : app_exception_base{ ::std::move( prm_message ) } {
		}

		/// \brief Ctor from the message as char *
		explicit precondition_error(char const * const &prm_message ///< The message to store in the exception
		                            ) : app_exception_base{ prm_message } {
		}

		~precondition_error() noexcept override = default;
		precondition_error(const precondition_error &) = default;
		precondition_error(precondition_error &&) = default;
		precondition_error & operator=(const precondition_error &) = default;
		precondition_error & operator=(precondition_error &&) = default;
	};

} } // namespace my::stuff

void deeper_fn() {
	TELL_THROW( my::stuff::precondition_error( "it has all gone terribly wrong" ) );
	std::cout
		<< "Here is a stack trace :\n"
		<< ::boost::stacktrace::stacktrace()
		<< "\n\n\n";

}

void intermediate_fn() {
	deeper_fn();
}

template <size_t other_value, typename T>
void fn(const T &value) {
	// intermediate_fn();
	BOOST_ASSERT(
		value < 1
	);
	BOOST_ASSERT_MSG(
		value < 1,
		( "The value must always be less than 1 because of rule X; it's currently " + ::std::to_string( value ) ).c_str()
	);
}

int main() {
	try {
		fn<3>( 5 );
	}
	catch (const my::stuff::app_exception_base &exception) {
		cerr << TELL_RETRIEVE_EXCEPTION_INFO( exception ) << flush;
	}
}

// -W -Wall -Werror -Wextra -pedantic -Wcast-qual -Wconversion -Wnon-virtual-dtor -Wshadow -Wsign-compare -Wsign-conversion

// clang++ -fcolor-diagnostics -I source/src_stacktrace -W -Wall -Werror -Wextra -pedantic -Wcast-qual -Wconversion -Wnon-virtual-dtor -Wshadow -Wsign-compare -Wsign-conversion -rdynamic -g -std=c++14 -stdlib=libc++ stacktrace.cpp -DBOOST_ENABLE_ASSERT_DEBUG_HANDLER -DBOOST_STACKTRACE_DYN_LINK -DBOOST_STACKTRACE_USE_ADDR2LINE -isystem /opt/boost_1_67_0_clang_c++14_build/include -Wl,-rpath,/opt/boost_1_67_0_clang_c++14_build/lib /opt/boost_1_67_0_clang_c++14_build/lib/libboost_stacktrace_addr2line-mt-d.so -o stacktrace.clang_addr2linebin && ./stacktrace.clang_addr2linebin
// g++                         -I source/src_stacktrace -W -Wall -Werror -Wextra -pedantic -Wcast-qual -Wconversion -Wnon-virtual-dtor -Wshadow -Wsign-compare -Wsign-conversion -rdynamic -g -std=c++14                stacktrace.cpp -DBOOST_ENABLE_ASSERT_DEBUG_HANDLER -DBOOST_STACKTRACE_DYN_LINK -DBOOST_STACKTRACE_USE_ADDR2LINE -isystem /opt/boost_1_67_0_gcc_c++14_build/include   -Wl,-rpath,/opt/boost_1_67_0_gcc_c++14_build/lib   /opt/boost_1_67_0_gcc_c++14_build/lib/libboost_stacktrace_addr2line-mt-d.so   -o stacktrace.gcc_addr2linebin   && ./stacktrace.gcc_addr2linebin

// g++                         -I source/src_stacktrace -W -Wall -Werror -Wextra -pedantic -Wcast-qual -Wconversion -Wnon-virtual-dtor -Wshadow -Wsign-compare -Wsign-conversion           -g -std=c++14                stacktrace.cpp -DBOOST_ENABLE_ASSERT_DEBUG_HANDLER -DBOOST_STACKTRACE_DYN_LINK -DBOOST_STACKTRACE_USE_BACKTRACE -isystem /opt/boost_1_67_0_gcc_c++14_build/include   -Wl,-rpath,/opt/boost_1_67_0_gcc_c++14_build/lib   /opt/boost_1_67_0_gcc_c++14_build/lib/libboost_stacktrace_backtrace-mt-d.so   -o stacktrace.gcc_backtrace_bin  && ./stacktrace.gcc_backtrace_bin
//
