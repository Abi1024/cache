CXX=g++
CXXFLAGS=
LDFLAGS=
SRCS=mm.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

matrixmultiply: $(OBJS)
	$(CXX) $(CXXFLAGS) -o matrixmultiply $(OBJS) $(LDFLAGS)

debug: CXXFLAGS += -D debug -g
debug: matrixmultiply

mm.o: mm.cpp 
	$(CXX) $(CXXFLAGS) -c mm.cpp


clean:
	-rm $(OBJS)
