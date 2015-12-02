CFLAGS=-std=c++98 -pthread -O2
PFLAGS=-Wl,--no-as-needed 
NFLAGS=-lrt

SOURCES=main.cpp Module.cpp

OBJECTS=$(SOURCES: %.cpp=%.o)
EXECUTABLE=simulator

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(CFLAGS) $(OBJECTS) -o $@ $(PFLAGS) $(NFLAGS)

$(OBJECTS): $(SOURCES)
	$(CXX) $(CFLAGS) $< -c $@ $(LDFLAGS)

clean:
	rm $(EXECUTABLE) *.o
