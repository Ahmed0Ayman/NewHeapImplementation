# NewHeapImplementation


this is concisely recap  of this library  

as we know heap in standard c library has a several problems like fragmentation and slower operation

in order to solve these problems we implementing our own version of heap as I trying to do today

so today I'll try to introduce my version of heap this first version use linked list to track the freed blocks

and if the algorithm found that there's two contiguous free blocks then it will combine these to block in one block

you also must know that this library need also you to assign an heap in linker script on your application . so you can use this library

on your application as optional and with the application that suffering from fragmentation .

so if you need to work with this library only two files you need to add in you application heap.c & heap.h 

after that you can found APIs to interface with this heap in heap.h file

I'm trying to solve any bugs and test my code so for any modification you can contact my to make this project reliable and bug free


