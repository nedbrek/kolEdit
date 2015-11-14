CXX ?= g++
CXXFLAGS += -MP -MMD -Wall -g -O3 -fPIC -I/usr/include/tcl8.5
LDFLAGS += -Wall

# build lib
LIBSRC := kol.cpp
LIBOBJ := $(LIBSRC:.cpp=.o)
LIBDEP := $(LIBOBJ:.o=.d)

.PHONY: all
all: kolEdit.exe kolEdit.dll

kolEdit.exe: $(LIBOBJ) main.o

kolEdit.dll: $(LIBOBJ) kolTcl.o -ltcl8.5

.cpp.o:
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

%.exe:
	@$(CXX) -o $@ $^ $(LDFLAGS)

%.dll:
	@$(CXX) -o $@ $^ $(LDFLAGS) -shared

.PHONY: clean
clean:
	@rm -f $(OBJ) $(DEP) kolEdit.exe

