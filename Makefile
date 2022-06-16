all: myfs.o mylibc.o

myfs.o : myfs.c
	gcc -c myfs.c -Wall
	
mylibc.o : Mylibc.c
	gcc -c Mylibc.c -Wall


clean: 
	rm -f *.o