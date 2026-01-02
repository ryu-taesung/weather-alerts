# Variables
CXX = g++
CXXFLAGS = -O3 -Wall -Wextra -std=c++17
GIT_VERSION := $(shell git describe --always --tags)

# Adding conditional flags for Raspberry Pi/armv6
ifeq ($(shell uname -m),armv6l)  # or "ifneq (,$(findstring arm, $(shell uname -m)))" for more general check
CXXFLAGS += -Wl,--no-keep-memory
endif

LIBS = -lcurl -lpthread -lboost_program_options
TARGET = weather-alerts
SOURCES = weather-alerts.cpp

# Rules
all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -DGIT_VERSION=\"$(GIT_VERSION)\" -o $@ $(INCLUDES) $^ $(LIBS)

clean:
	rm -f $(TARGET)
