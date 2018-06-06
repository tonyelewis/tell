# tell

Tony E Lewis's C++ Library

## Development

Why having to tell `clang-tidy` to use include directory `/opt/clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04/include/c++/v1`?

Ensure all headers compile

~~~bash
lsc | grep .hpp | xargs -IVAR clang++ -std=c++14 -stdlib=libc++ -fsyntax-only -fcolor-diagnostics -I source/src_stacktrace -W -Wall -Werror -Wextra -pedantic -Wcast-qual -Wconversion -Wnon-virtual-dtor -Wshadow -Wsign-compare -Wsign-conversion -isystem /opt/boost_1_67_0_clang_c++14_build/include VAR

lsc | grep .hpp | xargs -IVAR clang-tidy '-checks=*' VAR -- -std=c++14 -stdlib=libc++ -I source/src_stacktrace -isystem /opt/boost_1_67_0_clang_c++14_build/include -isystem /opt/clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04/include/c++/v1

lsc | grep .hpp | xargs -IVAR clang-tidy -checks=llvm-header-guard -fix VAR -- -x c++ -std=c++14 -stdlib=libc++ -I source/src_stacktrace -isystem /opt/boost_1_67_0_clang_c++14_build/include -isystem /opt/clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04/include/c++/v1
lsc | grep .hpp | xargs -IVAR clang-tidy -checks=google-readability-namespace-comments -fix VAR -- -x c++ -std=c++14 -stdlib=libc++ -I source/src_stacktrace -isystem /opt/boost_1_67_0_clang_c++14_build/include -isystem /opt/clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04/include/c++/v1
~~~
