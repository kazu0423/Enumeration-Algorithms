SRCS	 = main.cpp Enum.cpp
CXX		 = g++-8
CXXFLAGS = -std=c++14 -O3
EXE		 = EIMS

EIMS:$(SRCS)
	$(CXX)  $(SRCS) -o $@ $(CXXFLAGS)

debug:$(SRCS)
	$(CXX) $(CXXFLAGS) -D_GLIBCXX_DEBUG $(SRCS) -o $@

clean:
	-rm -f EIMS
