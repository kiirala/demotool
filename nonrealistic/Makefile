GENERAL_FLAGS=-O1 -march=native -Wall -Wextra -std=c++0x --fast-math -DNDEBUG -mfpmath=387
CXXFLAGS=`pkg-config --cflags sdl` -I. $(GENERAL_FLAGS)
LDFLAGS=-lGLEW

STATIC=nonrealistic
DYNOBJ=renderer.so
DYNREQS=renderer.o edges.o noise.o globject.o glsupport.o logger.o matrix.o mesh.o phongmodel.o

ASM=yasm
ASMFLAGS=-m amd64 -f elf -w-macro-params
LD=ld
ASMLINK=-ldl -lstdc++ -L/usr/lib/gcc/x86_64-pc-linux-gnu/4.4.5 -lSDL -dynamic-linker /lib64/ld-linux-x86-64.so.2

all: $(STATIC) $(DYNOBJ) asmtree smalltree sdlsmall

smalltree: asmtree
	strip $^ -R .comment -R.gnu.version -R .eh_frame -o $@-strip
	sstrip $@-strip
	gzip -9 -n $@-strip
	echo "#!/bin/sh" >$@
	echo "a=/tmp/I;tail -n+3 \$$0|zcat>\$$a;chmod +x \$$a;\$$a;rm \$$a;exit" >>$@
	cat $@-strip.gz >>$@
	chmod +x $@
	rm $@-strip.gz

sdlsmall: nonrealistic
	strip $^ -R .comment -R.gnu.version -R .eh_frame -o $@-strip
	sstrip $@-strip
	gzip -9 -n $@-strip
	echo "#!/bin/sh" >$@
	echo "a=/tmp/I;tail -n+3 \$$0|zcat>\$$a;chmod +x \$$a;\$$a \"\$$@\";rm \$$a;exit" >>$@
	cat $@-strip.gz >>$@
	chmod +x $@
	rm $@-strip.gz

asmtree: asminit.o ${DYNREQS}
	${LD} ${LDFLAGS} ${ASMLINK} -o $@ $^

asminit.o: asminit.asm sdlinc.asm sizes.asm config.asm
	${ASM} ${ASMFLAGS} $< 

$(STATIC): $(DYNREQS) sdlmain.o
	$(CXX) `pkg-config --libs sdl` $(LDFLAGS) $^ -o $@

$(DYNOBJ): $(DYNREQS) ../common/common.a
	$(CXX) -shared $(LDFLAGS) $(GENERAL_FLAGS) $^ -o $@

$(DYNREQS): %.o: %.cpp $(IMAGES)
	$(CXX) -fPIC -c $(CXXFLAGS) $< -o $@

$(IMAGES): %.h: %.svg
	$(MAKE) -C img

-include $(DYNREQS:.o=.d)

clean:
	rm -f $(STATIC) $(DYNOBJ) $(DYNREQS) $(DYNREQS:.o=.d) *~
