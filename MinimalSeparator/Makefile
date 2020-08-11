SRCS	 = main.cpp
CXX		 = g++
CXXFLAGS = -std=c++14 -O3
EXE		 = EMS

EMS:$(SRCS)
	$(CXX)  $(SRCS) -o $@ $(CXXFLAGS)

debug:$(SRCS)
	$(CXX) $(CXXFLAGS) -D_GLIBCXX_DEBUG $(SRCS) -o $@

clean:
	-rm -f EIS
