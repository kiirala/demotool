GENERAL=-Os -g -march=native -Wall -Wextra
CXXFLAGS=`pkg-config --cflags gtkmm-2.4 gtkglext-1.0` $(GENERAL) -MMD -MP
LDFLAGS=`pkg-config --libs gtkmm-2.4 gtkglext-1.0` $(GENERAL)

OBJECTS=main.o datastorage.o moduleloader.o gui.o
PROGRAM=main
DYNOBJ=renderer.so
DYNREQS=renderer.o

all: $(PROGRAM) $(DYNOBJ)

$(PROGRAM): $(OBJECTS)

$(DYNOBJ): $(DYNREQS)
	$(CXX) -shared $(LDFLAGS) $^ -o $@

$(DYNREQS): %.o: %.cpp
	$(CXX) -fPIC -c $(CXXFLAGS) $< -o $@

-include $(OBJECTS:.o=.d) $(DYNREQS:.o=.d)

clean:
	rm -f $(PROGRAM) $(OBJECTS) $(DYNOBJ) $(OBJECTS:.o=.d) $(DYNREQS:.o=.d) *~
