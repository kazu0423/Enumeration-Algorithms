SRCS	 = main.cpp enum.cpp graph.cpp
CXX		 = g++
# CXXFLAGS = -std=c++17 -O3 -fdiagnostics-color -I ~/boost_1_63_0 
# CXXFLAGS = -std=c++17 -O3
CXXFLAGS = -std=c++17 -O3
EXE		 = EHIS

EHIS:$(SRCS)
	$(CXX)  $(SRCS) -o $@ $(CXXFLAGS)

debug:$(SRCS)
	$(CXX) $(CXXFLAGS) -D_GLIBCXX_DEBUG $(SRCS) -o $@

clean:
	-rm -f EHIS
