export GENERAL_FLAGS=-Os -g -march=native -Wall -Wextra -std=c++0x -MMD -MP

PROGRAMS=main sdlmain.o
PROJECTS=gospace
COMMON=common/common.a

all: $(PROGRAMS) $(PROJECTS) $(COMMON)

.PHONY: $(PROJECTS)

$(PROGRAMS):
	$(MAKE) -C tool $@
	cp tool/$@ .

$(PROJECTS): $(COMMON) $(PROGRAMS)
	$(MAKE) -C $@

$(COMMON):
	$(MAKE) -C common

clean:
	rm -f $(PROGRAMS)
	$(MAKE) -C tool clean
	$(MAKE) -C common clean
	for i in $(PROJECTS) ; do make -C $i clean ; done

