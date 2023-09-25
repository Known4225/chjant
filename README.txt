Seagate is a C compiler with limited capability.
It will compile the seagateFunction.c file into a logic gates txt file that can be opened with my logic gates application (included in this project)

Write C code in the seagateFunction.c file and then run seagate.o.
The code will generate a text file that can then be loaded into the logic gates simulator.

You can use make to make the seagate program, or use
gcc seagate.c recordPaddedReg.c include/list.c -o seagate.o
because i decided to overcomplicate it this time and have it use multiple c files

Current features include:
Bitwise NOT, AND, and OR
Parentheses parsing
Assigning new variables and redefining old ones
Logical operators

ToAdd:
Increments and Decrements
Shifts left and right
Addition and subtraction
Multiplication
Division and Modulo
Funky assignment operations
Loops (?) - may be impossible, I guess I could incorporate a clock, but then I'd need memory and at that point I might as well just build a computer


To be honest, it's a miracle that this thing barely works in its current state
I never made a C parser or compiler before, and that should be fairly obvious with one glance of the code









/* logic gates information */


Logic Gates is a drag and drop circuit builder for creating and testing circuits built from gates.
Choose from 8 different unique components and build the computational world of your dreams.

Compile (windows 64 bit):
gcc logicgates.c -L./Windows -lglfw3 -lopengl32 -lgdi32 -O3 -lglad -o logicgates.exe

Compile (linux):
gcc logicgates.c -L./Linux -lglfw3 -ldl -lm -lX11 -lglad -lGL -lGLU -lpthread -O3 -o logicgates.o

Then run logicgates.exe

I've already included the exe and object files as well, so you don't need to compile if they already work (no guarentees though, this stuff is very finicky)

To load a file, type:
logicgates.exe {filename.txt}
in the terminal (with no brackets)
I've included a seven segment display as well as a 4 bit Arithmetic Logic Unit

Keybinds:
click and drag - Place components, move components, or move screen
click on a POWER component to toggle it on/off
space + click + drag - create wire
1, e, p - POWER component (input/output)
2, w, n - NOT gate
3, a - AND gate
4, q, o - OR gate
5 - XOR gate
6 - NOR gate
7 - NAND gate
8 - BUFFER gate
x - delete component
shift + click + drag - select
shift + click - add component to selection
c - copy/paste selected
scroll wheel (or up and down arrows) - zoom
up and down arrows - zoom
space + scroll wheel - rotate component (coarse)
sideways arrows - rotate component (fine)
space + sideways arrows - rotate selected (fine)
k - export to file
h - toggle sidebar
t - toggle theme (dark/light)
z - snap to grid