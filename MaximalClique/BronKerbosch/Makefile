SRCS	 = main.cpp Enum.cpp
CXX		 = g++-7
CXXFLAGS = -std=c++14 -O3
EXE		 = EMC

EMC:$(SRCS)
	$(CXX)  $(SRCS) -o $@ $(CXXFLAGS)

debug:$(SRCS)
	$(CXX) $(CXXFLAGS) -D_GLIBCXX_DEBUG $(SRCS) -o $@

clean:
	-rm -f EMC
