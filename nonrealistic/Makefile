CXXFLAGS=`pkg-config --cflags sdl` -I. $(GENERAL_FLAGS)
LDFLAGS=-lGLU -lGLEW $(GENERAL_FLAGS)

STATIC=nonrealistic
DYNOBJ=renderer.so
DYNREQS=renderer.o edges.o noise.o globject.o glsupport.o logger.o matrix.o mesh.o phongmodel.o

all: $(STATIC) $(DYNOBJ)

$(STATIC): $(DYNREQS) sdlmain.o
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
