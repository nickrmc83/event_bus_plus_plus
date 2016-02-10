event_bus_test : event_bus_test.cpp event_bus.hpp
	c++ -std=c++11 -g -O0 -o $@ $< -pthread
