# /home/ilya/TestFlashSim/OriginalFlashSim/Makefile
CXX=g++
CXXFLAGS=-Wall -c -std=c++11 -g
LDFLAGS=
HEADERS=ssd.h tests/tests.h

SOURCES_SSDLIB = $(filter-out ssd_ftl.cpp, $(wildcard ssd_*.cpp))  \
                 $(wildcard FTLs/*.cpp)                            \
                 $(filter-out tests/run_%.cpp, $(wildcard tests/*.cpp)) \
                 zns_manager.cpp                                   \
                 crypto_engine.cpp                                 \
                 eeprom_manager.cpp                                \
                 thermal_manager.cpp                               \
                 SSDSim.cpp

OBJECTS_SSDLIB=$(patsubst %.cpp,%.o,$(SOURCES_SSDLIB))
SOURCES_RUNS = $(wildcard run_*.cpp)
PROGRAMS = $(patsubst run_%.cpp,%,$(SOURCES_RUNS))

all: $(PROGRAMS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $< -o $@

FTLs/%.o: FTLs/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $< -o $@

tests/%.o: tests/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $< -o $@

define PROGRAM_TEMPLATE
  $1 : run_$1.o $$(OBJECTS_SSDLIB)
	$$(CXX) $$(LDFLAGS) $$< $$(OBJECTS_SSDLIB) -o $$@
endef

$(foreach prog,$(PROGRAMS),$(eval $(call PROGRAM_TEMPLATE,$(prog))))

clean:
	-rm -rf *.o FTLs/*.o tests/*.o $(PROGRAMS)

.PHONY: files
files:
	@echo $(SOURCES_SSDLIB) $(SOURCES_RUNS) $(HEADERS) | tr ' ' '\n'
