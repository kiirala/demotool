CXXFLAGS=`pkg-config --cflags sdl` -I. -I../common $(GENERAL_FLAGS) -MMD -MP
LDFLAGS=-lGLU -lGLEW -lSDL_sound $(GENERAL_FLAGS)

STATIC=gospace
DYNOBJ=renderer.so
DYNREQS=renderer.o superformula.o caleidoscope.o sunrise.o
IMAGES=$(patsubst %.svg,%.h,$(wildcard img/*.svg))

all: $(STATIC) $(DYNOBJ)

$(STATIC): $(DYNREQS) ../tool/sdlmain.o ../common/common.a 
	$(CXX) `pkg-config --libs sdl` $(LDFLAGS) $^ -o $@

$(DYNOBJ): $(DYNREQS) ../common/common.a
	$(CXX) -shared $(LDFLAGS) $^ -o $@

$(DYNREQS): %.o: %.cpp $(IMAGES)
	$(CXX) -fPIC -c $(CXXFLAGS) $< -o $@

$(IMAGES): %.h: %.svg
	$(MAKE) -C img

-include $(DYNREQS:.o=.d)

clean:
	rm -f $(STATIC) $(DYNOBJ) $(DYNREQS) $(DYNREQS:.o=.d) *~
