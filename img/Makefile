JAVAC=javac
JAVA=java
CONFIG=-classpath .:`java-config -p batik-1.6,xerces-2`

TARGETS=Canvas.class DrawSVG.class SimpleUserAgent.class
IMAGES=$(patsubst %.svg,%.h,$(wildcard *.svg))

all: $(TARGETS) $(IMAGES)

$(TARGETS): %.class: %.java
	javac $(CONFIG) $<

$(IMAGES): %.h: %.svg DrawSVG.java
	java $(CONFIG) DrawSVG $<