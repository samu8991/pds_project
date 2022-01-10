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
This project can be executed either on windows or linux os.


### Linux

Software needed:
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
### Windows

//TODO GIACOMO






---

## Usage <a name="usage"></a>

Regardless of whether you are using linux or windows, you have to put the
benchmarks folder inside a specific folder.
As far as windows is concerned 
the folder is: "C:" while in linux is: "/home/username" or
the well known "~".

