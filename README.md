google
======

##Google command line search tool
Disclaimer: This is just a project where I'm playing with C for fun.
**BEFORE YOU WASTE YOUR TIME THIS NO LONGER WORKS, GOOGLE REMOVED THE API IT RELIES ON**

### Building

To build the project first fetch the git submodules then run cmake and make in the build dir
```
git submodule init
git submodule update
cd build
cmake ..
make
```
### Usage
Running google with no arguments will return the help
```
$ ./google
Google Search Tool:
Usage:   google <search query>
Example: google github
Example: google "blah blah blah"
Example: google blah\ blah\ blah
```
