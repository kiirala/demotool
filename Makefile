GENERAL=-Os -g -march=native -Wall -Wextra
CXXFLAGS=`pkg-config --cflags gtkmm-2.4 gtkglext-1.0` $(GENERAL) -MMD -MP
LDFLAGS=`pkg-config --libs gtkmm-2.4 gtkglext-1.0` $(GENERAL)

OBJECTS=main.o datastorage.o moduleloader.o scheduler.o gui.o
PROGRAM=main

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)

-include $(OBJECTS:.o=.d)

clean:
	rm -f $(PROGRAM) $(OBJECTS) $(OBJECTS:.o=.d) *~
