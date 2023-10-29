# Variables
CXX = g++
CXXFLAGS = -O3 -Wall -std=c++17
INCLUDES = -I../boost_1_83_0
LIBS = -lcurl -lpthread -L../boost_1_83_0/stage/lib -lboost_program_options
TARGET = weather-alerts
SOURCES = weather-alerts.cpp

# Rules
all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $(INCLUDES) $^ $(LIBS)

clean:
	rm -f $(TARGET)
