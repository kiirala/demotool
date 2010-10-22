GENERAL=-Os -g -march=k8 -Wall -Wextra -std=c++0x
CXXFLAGS=`pkg-config --cflags gtkmm-2.4 gtkglext-1.0 sdl` -I. $(GENERAL) -MMD -MP
LDFLAGS=-lGLEW -lSDL_sound $(GENERAL)

OBJECTS=main.o datastorage.o moduleloader.o gui.o
PROGRAM=main
SDLOBJECTS=sdlmain.o
SDLPROGRAM=sdlmain
DYNOBJ=renderer.so
DYNREQS=renderer.o superformula.o mesh.o matrix.o logger.o globject.o texturepreview.o caleidoscope.o image.o triangulate.o sunrise.o
IMAGES=$(patsubst %.svg,%.h,$(wildcard img/*.svg))

all: $(PROGRAM) $(SDLPROGRAM) $(DYNOBJ)

$(SDLPROGRAM): $(SDLOBJECTS) $(DYNREQS)
	$(CXX) `pkg-config --libs sdl` $(LDFLAGS) $^ -o $@

$(PROGRAM): $(OBJECTS)
	$(CXX) `pkg-config --libs gtkmm-2.4 gtkglext-1.0` $(LDFLAGS) $^ -o $@

$(DYNOBJ): $(DYNREQS)
	$(CXX) -shared $(LDFLAGS) $^ -o $@

$(DYNREQS): %.o: %.cpp $(IMAGES)
	$(CXX) -fPIC -c $(CXXFLAGS) $< -o $@

$(IMAGES): %.h: %.svg
	$(MAKE) -C img

-include $(OBJECTS:.o=.d) $(DYNREQS:.o=.d)

clean:
	rm -f $(PROGRAM) $(SDLPROGRAM) $(SDLOBJECTS) $(OBJECTS) $(DYNOBJ) $(DYNREQS) $(OBJECTS:.o=.d) $(DYNREQS:.o=.d) *~
