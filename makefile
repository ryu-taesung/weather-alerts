# Variables
CXX = g++
CXXFLAGS = -Os -Wall -Wextra -std=c++17
# Adding conditional flags for Raspberry Pi/armv6
ifeq ($(shell uname -m),armv6l)  # or "ifneq (,$(findstring arm, $(shell uname -m)))" for more general check
CXXFLAGS += -Wl,--no-keep-memory
endif
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
