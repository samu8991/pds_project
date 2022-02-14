# ParallelGraphColoring

---

This project is about solving a famous problem in informatics called vertex coloring problem.
The problem is defined as follows: given a graph G=(V,E) where V is the set of nodes and E is the set
of edges of the graph you have to find a function C
: V &#8594; N capable of assigning a color to each vertex
such that for each (u,v) in E C(v) <> C(u).

## Contents

1. [Build instructions](#building)
   1. [Linux](#linux_build)
   2. [Windows](#windows_build)
2. [Usage](#usage)
---
## Build instructions <a name="building"></a>
This project can be run on both windows and linux operating systems

### Linux <a name="linux_build"></a>

Software needed:
- Download Boost from the [official site](https://www.boost.org/users/download/)
- cmake(>=3.17)
- make
- boost filesystem library

On debian distributions you can run the following:

```
# apt install cmake libboost-filesystem-dev
```

In order to compile you have to put yourself inside the 
build directory and then hit the following:

```
$ cmake .. && make
```
### Windows <a name="windows_build"></a>

Software needed:
- boost library

On Windows distribution compute the following instruction:
- Download Boost from the [official site](https://www.boost.org/users/download/)
- Extract Boost from the *.7z* folder
- Open the Boost folder and run *bootstrap.bat*; it creates the file *b2.exe*
- Run *b2.exe* and wait it finishes

In order to compile you have to put yourself inside the 
build directory and then hit the following:

```
$ cmake .. && make
```
---

## Usage <a name="usage"></a>

Regardless of whether you are using linux or windows, you must put the
benchmark folder inside a specific folder.
As far as windows is concerned 
the folder is: "C:" while in linux is: "/home/username" or
the well known "~".
There are actually two ways of executing the program:
1. without arguments, the program will guide the user
2. with arguments(representation,algorithm,no. threads, file name). 



