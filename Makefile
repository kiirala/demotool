export GENERAL_FLAGS=-Os -g -march=native -Wall -Wextra -std=c++0x -MMD -MP

PROGRAMS=main sdlmain.o
PROJECTS=gospace depthpeeling
COMMON=common/common.a

all: demotool $(PROGRAMS) $(PROJECTS) $(COMMON)

demotool: tool/main
	cp tool/main demotool

tool/main: main ;

.PHONY: $(PROGRAMS) $(PROJECTS) $(COMMON) clean

$(PROGRAMS):
	$(MAKE) -C tool $@

$(PROJECTS): $(COMMON) $(PROGRAMS)
	-$(MAKE) -C $@

$(COMMON):
	$(MAKE) -C common

clean:
	rm -f demotool
	$(MAKE) -C tool clean
	$(MAKE) -C common clean
	for i in $(PROJECTS) ; do make -C $$i clean ; done

