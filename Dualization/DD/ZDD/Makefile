CXX := g++
CXXFLAGS := -O3 -std=c++17 -Wall -Wextra -DB_64

# Set these to match your SAPPOROBDD installation.
SBDD_INC ?= /Users/kuritakazuhiro/.dd_libs/SAPPOROBDD/include
SBDD_LIB ?= /Users/kuritakazuhiro/.dd_libs/SAPPOROBDD/lib
SBDD_LIBS ?= /Users/kuritakazuhiro/.dd_libs/SAPPOROBDD/lib/BDD64.a
SBDDH_INC ?= /Users/kuritakazuhiro/.dd_libs/sbdd_helper

TARGET := main
SRCS := main.cpp
OBJS := $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -L$(SBDD_LIB) $(SBDD_LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(SBDD_INC) -I$(SBDDH_INC) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
