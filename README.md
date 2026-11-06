## MBUILD
mbuild is an automation build system which is created in the consideration of build process of large code bases and simplify the build processes. and also for managing nested builds.

### Design

* the design is very user friendly specially if you have used make file before. it is just you write targets once and do the commands your self like: 
```
list-files:
    ls -la
    
``` 
* If you want to use groups just do the target name and put your arrays of targets in [] but even the last one must contain **','** infront of it and there is no space allowed between members of your group array, And this target runs the target of its own Mbuildfile file not other targets of Mbuildfile files in other directories!

```
build-kernel:
  [compile-kernel,compile-some-other-so,compile-syscalls,]
```

but don't forget to insert new line at the end the build file

* you must create the build file in your working directory called "Mbuildfile"
* spaces are not allowed. it must be specifically a tab following the ':' after the target name to type the command


### Requirements
-> linux kernel

-> libc

-> glibc malloc memory grabing size at least



### installation
-> make sure you clone the repo

```bash
    git clone https://github.com/message-beast/mbuild
    cd mbuild

```
```bash
    sudo make compile
```

#### more info?
run:
```bash
    mbuild --help
```
