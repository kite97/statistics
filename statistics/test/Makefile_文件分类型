CC = gcc
OBJECTS = main.o wave.o mp3.o amr.o
INC = -I include
objects := $(wildcard *.o)
vpath %.c src


statistics: $(OBJECTS) 
	$(CC) -o statistics $(OBJECTS)



$(OBJECTS) : %.o:%.c 
	$(CC) -c $< $(INC) -o $@


.PHONY: cleanall cleanexe cleanobj
cleanall:
	rm -f statistics $(objects)
cleanexe:
	rm -f statistics
cleanobj:
	rm -f *.o
