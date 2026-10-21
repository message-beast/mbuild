## MBUILD
mbuild is an automation build system which is created in the consideration of build process of large code bases and simplify the build processes. and also for managing nested builds.

### Design

* the design is very user friendly specially if you have used make file before. it is just you write targets once and do the commands your self like: 
```
list-files:
    ls -la
    
``` 
but don't forget to insert new line at the end the build file

* you must create the build file in your working directory called "Mbuildfile"
* spaces are not allowed. it must be specifically a tab following the ':' after the target name to type the command


### Requirements
-> linux kernel

-> libc

-> glibc malloc memory grabing size at least



### installation
```bash
    sudo make compile
```

#### more info?
run:
```bash
    mbuild --help
```