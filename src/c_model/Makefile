HEADERS = primate_numeric.h primate_p4.h p4_parse_simple.h

SOURCES = p4_parse_sw.cpp

DEPS = \
	Makefile \
	$(HEADERS) \
	$(SOURCES)

all: main

main: $(DEPS)
	g++ -o $@ $(SOURCES)

.PHONY: clean
clean:
	rm main

