SRCS	 = main.cpp
CXX		 = mpicxx
CXXFLAGS = -std=c++14 -O3
EXE		 = LCM

LCM:$(SRCS)
	$(CXX)  $(SRCS) -o $@ $(CXXFLAGS)

debug:$(SRCS)
	$(CXX) $(CXXFLAGS) -D_GLIBCXX_DEBUG $(SRCS) -o $@

clean:
	-rm -f LCM
