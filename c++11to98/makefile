CFLAGS=-std=c++11
LDFLAGS=-pthread 
PFLAGS=-Wl,--no-as-needed

SOURCES=main.cpp Module.cpp Queue.cpp Socket.cpp

OBJECTS=$(SOURCES: %.cpp=%.o)
EXECUTABLE=simulator

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS) $(PFLAGS)

$(OBJECTS): $(SOURCES)
	$(CXX) $(CFLAGS) $< -c $@ $(LDFLAGS)

clean:
	rm $(EXECUTABLE) *.o
