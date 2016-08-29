# Author: Marcus Ross
#         MR867434

p3: clean main.o init.o query.o
	gcc main.o init.o query.o -o p3
main.o: main.c proto.h dbStruct.h
	gcc -c main.c
init.o: init.c dbStruct.h const.h
	gcc -c init.c
query.o: query.c dbStruct.h proto.h
	gcc -c query.c
clean: 
	rm -f *.o core