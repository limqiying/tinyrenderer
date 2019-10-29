CXX = g++-9
INCLUDES = -I/usr/local/include/eigen3
SRCS = main.cpp model.cpp tgaimage.cpp
# OBJS = $(SRCS:.cpp=.o)
MAIN = a.out

all:    $(MAIN)
	@echo Compilation successful

$(MAIN):	$(SRCS)
	$(CXX) $(INCLUDES) $(SRCS) -o $(MAIN)

clean:
	rm *.o
	rm *.out