# MIPS Virtual Machine
Runs machine code for MIPS instruction set, no floating point registers
## Download instructions
```
git clone https://github.com/keanu-thakalath/mips.git
cd mips
```

#### Compile and run example with g++
```
g++ -o mips.exe main.cpp MIPS.cpp
./mips.exe machine-code\sample-program
```
## Syscalls
The syscalls are described [here](http://courses.missouristate.edu/KenVollmar/Mars/Help/SyscallHelp.html)
```
print integer   (0x01)
print string    (0x04)
read integer    (0x05)
read string     (0x08)
sbrk            (0x09)
exit            (0x0A)
print character (0x0B)
read character  (0x0C)
exit2           (0x11)
```
are the ones that are implemented.
