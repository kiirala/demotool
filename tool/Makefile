CXXFLAGS=`pkg-config --cflags gtkmm-2.4 gtkglext-1.0 sdl` -I. $(GENERAL_FLAGS) -MMD -MP
LDFLAGS=$(GENERAL_FLAGS)

OBJECTS=main.o moduleloader.o gui.o
PROGRAM=main
SDLOBJECTS=sdlmain.o

all: $(PROGRAM) $(SDLOBJECTS)

$(PROGRAM): $(OBJECTS)
	$(CXX) `pkg-config --libs gtkmm-2.4 gtkglext-1.0` $(LDFLAGS) $^ -o $@

-include $(OBJECTS:.o=.d) $(SDLOBJECTS:.o=.d)

clean:
	rm -f $(PROGRAM) $(SDLOBJECTS) $(OBJECTS) $(OBJECTS:.o=.d) $(SDLOBJECTS:.o=.d) *~
