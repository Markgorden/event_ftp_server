OBJS = $(wildcard ./*.o)
CC := g++

all : $(OBJS)
	$(CC) $^ -o $(TARGET) $(LDFLAGS) 
	@cp $(TARGET) ..
	@echo "ld done!"

.PHONY : clean

clean:
	rm -rf *.o $(TARGET)

