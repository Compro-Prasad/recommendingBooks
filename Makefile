CFLAGS = -std=c++14

TARGETS = combineCSV displayCSV

HEADERS = src/csv.hpp src/common.hpp

.PHONY: clean all

all: $(TARGETS)

clean:
	-rm -f $(TARGETS) *.o

combineCSV: src/combineCSV.cpp $(HEADERS)
	g++ $(CFLAGS) -o $@ $<

displayCSV: src/displayCSV.cpp $(HEADERS)
	g++ $(CFLAGS) -o $@ $<
