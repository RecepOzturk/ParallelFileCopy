# ParallelFileCopy

As asynchron reading from a file and writing another file with multi threads.

It was published to help students or interested persons with multithread and asynchron i/o.

To compile you have to entire this codes orderly;

gcc -lrt -c main.c -lpthread

gcc main.o -lrt -o main.out -lpthread

./main.out - - 8 (Put a space between each parameter.)

dash(-) mean; source and destination file will created in currently direction where place in main.c

You will copy the content of source file (source.txt) to a new destination file (destination.txt).
  You will take 3 input parameter in terminal.
*Source path (- (dash) means same path as executable file)
*Destination path (- (dash) means same path as executable file)
*Number of threads

Source and destination paths can be given as real paths OR as the character (– dash) which means that related txt file will be created under the same path as executable file.
I were defined a function to prepare a source file randomly. The content of the source file must be readable. Source file is created in main thread.

Source file has size of 60 Byte initially. You can change this when you want.

For example; let’s say that the user enters 4 threads and the size of the source file as 8 bytes. So each thread must copy 2 bytes for each. In order to be readable, your source file must be created randomly as “abcdefgh”.
o 1st thread should copy from source.txt to destination.txt first 2 bytes > ab
o 2nd thread should copy from source.txt to destination.txt second 2 bytes >cd

But 4 thread and source file as 10 byte?
Then first 3 thread has 2 byte each one, last thread has 4 byte. Or 6 thread 50 byte, first 5 thread has 8 byte and last thread has 10 bute.








