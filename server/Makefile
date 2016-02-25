MODULE := server
SRC := $(wildcard *.cpp)
OBJS := $(SRC:.cpp=.o)
CC := g++

all : $(MODULE)
	@echo "compile $(MODULE) done"

$(MODULE) : $(OBJS)
	@mv $(OBJS) $(OBJ_DIR)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $^

.PHONY : clean

clean:
	rm -rf $(OBJS) $(MODULE)
