UM-32 Virtual Machine
=====================

A UM-32 "Universal Machine" VM written without any consideration for performance
what-so-ever. Could have inlined many functions and passed by reference to avoid
many memory copies.. or singleton'd this VM into static memory, but did not want
to at the expense of readability and portability.

UM-32 is a machine designed for the 2006 ICFP Programming Contest. The first
part of the contest involves the implementation of this machine to run a
supplied program.

A physical machine specifications are detailed in: um-spec.txt

More information can be found here: http://www.boundvariable.org/task.shtml


Building
========

```bash
make

./um32.out <program>
```

You may need libc6-dev-i386 installed:

```bash
apt-get install gcc-multilib
```
