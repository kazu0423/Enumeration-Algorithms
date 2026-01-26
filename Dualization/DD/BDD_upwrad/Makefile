CXX := g++
CXXFLAGS := -O3 -std=c++17 -Wall -Wextra -DB_64
CXXFLAGS_PROFILE := $(CXXFLAGS) -DPROFILE_TD

# Set these to match your SAPPOROBDD installation.
SBDD_INC ?= /Users/kuritakazuhiro/.dd_libs/SAPPOROBDD/include
TDZDD_INC ?= /Users/kuritakazuhiro/.dd_libs/TdZdd-master/include
SBDD_LIB ?= /Users/kuritakazuhiro/.dd_libs/SAPPOROBDD/lib
SBDD_LIBS ?= /Users/kuritakazuhiro/.dd_libs/SAPPOROBDD/lib/BDD64.a

TARGET := main
TARGET_PROFILE := main_prof
SRCS := main.cpp dual.cpp
OBJS := $(SRCS:.cpp=.o)
OBJS_PROFILE := $(SRCS:.cpp=_prof.o)

all: $(TARGET)

profile: $(TARGET_PROFILE)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -L$(SBDD_LIB) $(SBDD_LIBS)

$(TARGET_PROFILE): $(OBJS_PROFILE)
	$(CXX) $(CXXFLAGS_PROFILE) -o $@ $^ -L$(SBDD_LIB) $(SBDD_LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(SBDD_INC) -I$(TDZDD_INC) -c $< -o $@

%_prof.o: %.cpp
	$(CXX) $(CXXFLAGS_PROFILE) -I$(SBDD_INC) -I$(TDZDD_INC) -c $< -o $@

clean:
	rm -f $(OBJS) $(OBJS_PROFILE) $(TARGET) $(TARGET_PROFILE)

.PHONY: all profile clean
