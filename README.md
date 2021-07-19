# TaskRunner
libtaskrunner is a multi-platform support library with a focus on asynchronous tasks dispatcher. It was primarily developed by C++ for use message loop.

## Installation
```CMake
# Check out the library.
$ git clone https://github.com/panda-factory/taskrunner.git
# Go to the library root directory
$ cd taskrunner
# taskrunner requires logging as a dependency. Add the source tree as a subdirectory.
$ git submodule update --init --recursive
# or clone logging source to third-party
# $ git clone https://github.com/panda-factory/logging.git third-party/logging
# Make a build directory to place the build output.
$ cmake -E make_directory "build"
# Generate build system files with cmake.
$ cmake -E chdir "build" cmake -DCMAKE_BUILD_TYPE=Release ../
# Build the library.
$ cmake --build "build" --config Release
```
Done builds the taskrunner libraries. On a windows system, the build directory should now look something like this:
```
/taskrunner
  /lib
    /Release
      /taskrunner.lib
      ...
```
