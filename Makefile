CXX ?= g++
CXXFLAGS += -MP -MMD -Wall -g -O3
LDFLAGS += -Wall

# build lib
LIBSRC := kol.cpp
LIBOBJ := $(LIBSRC:.cpp=.o)
LIBDEP := $(LIBOBJ:.o=.d)

.PHONY: all
all: kolEdit.exe

kolEdit.exe: $(LIBOBJ) main.o

.cpp.o:
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

%.exe:
	@$(CXX) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	@rm -f $(OBJ) $(DEP) kolEdit.exe

