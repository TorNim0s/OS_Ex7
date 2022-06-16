all: 

myfs.o : myfs.c
	gcc -c myfs.c

clean: 
	rm -f *.o