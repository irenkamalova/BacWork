CFLAGS=-std=c++98
LDFLAGS=-pthread 
PFLAGS=-Wl,--no-as-needed

SOURCES=Parser.cpp Module.cpp

OBJECTS=$(SOURCES: %.cpp=%.o)
EXECUTABLE=simulator

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS) $(PFLAGS)

$(OBJECTS): $(SOURCES)
	$(CXX) $(CFLAGS) $< -c $@ $(LDFLAGS)

clean:
	rm $(EXECUTABLE) *.o
