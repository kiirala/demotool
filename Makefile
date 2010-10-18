GENERAL=-Os -g -march=native -Wall -Wextra -std=c++0x
CXXFLAGS=`pkg-config --cflags gtkmm-2.4 gtkglext-1.0` -I. $(GENERAL) -MMD -MP
LDFLAGS=`pkg-config --libs gtkmm-2.4 gtkglext-1.0` $(GENERAL)

OBJECTS=main.o datastorage.o moduleloader.o gui.o
PROGRAM=main
DYNOBJ=renderer.so
DYNREQS=renderer.o superformula.o mesh.o matrix.o logger.o globject.o texturepreview.o caleidoscope.o

all: $(PROGRAM) $(DYNOBJ)

$(PROGRAM): $(OBJECTS)

$(DYNOBJ): $(DYNREQS)
	$(CXX) -shared $(LDFLAGS) -lGLEW $^ -o $@

$(DYNREQS): %.o: %.cpp
	$(CXX) -fPIC -c $(CXXFLAGS) $< -o $@

-include $(OBJECTS:.o=.d) $(DYNREQS:.o=.d)

clean:
	rm -f $(PROGRAM) $(OBJECTS) $(DYNOBJ) $(DYNREQS) $(OBJECTS:.o=.d) $(DYNREQS:.o=.d) *~
