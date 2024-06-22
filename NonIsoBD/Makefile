SRCS	 = main.cpp Enum.cpp
CXX		 = g++-14
CXXFLAGS = -std=c++17 -O3
EXE		 = NIG

NIG:$(SRCS)
	$(CXX)  $(SRCS) -o $@ $(CXXFLAGS)

debug:$(SRCS)
	$(CXX) $(CXXFLAGS) -D_GLIBCXX_DEBUG $(SRCS) -o $@

clean:
	-rm -f NIG
