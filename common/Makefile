CXXFLAGS=$(GENERAL_FLAGS)

OBJECTS=globject.o image.o logger.o matrix.o mesh.o phongmodel.o texturepreview.o triangulate.o glsupport.o

all: common.a $(OBJECTS)

common.a: $(OBJECTS)
	$(AR) -rc $@ $^

$(OBJECTS): %.o: %.cpp
	$(CXX) -fPIC -c $(CXXFLAGS) $< -o $@

-include $(OBJECTS:.o=.d)

clean:
	rm -f $(OBJECTS) common.a $(OBJECTS:.o=.d) *~
