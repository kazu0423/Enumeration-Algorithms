SRCS	 = main.cpp enum.cpp bipartite.cpp
CXX		 = g++
CXXFLAGS = -std=c++17 -O3 -fdiagnostics-color -I ~/boost_1_63_0 
EXE		 = ELM

ELM:$(SRCS)
	$(CXX)  $(SRCS) -o $@ $(CXXFLAGS)

debug:$(SRCS)
	$(CXX) $(CXXFLAGS) -D_GLIBCXX_DEBUG $(SRCS) -o $@

clean:
	-rm -f ELM
