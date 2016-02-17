# This source code is governed by by the boost v1 licensing terms and conditions.
# Copyright 2016 Nicholas A. Smith (nickrmc83@gmail.com)

# To build the test application with a non-default c++ compiler set environment
# variable CXX. For example to build with clang++ on an Ubuntu machine try:
# make clean && CXX=clang++ make

TARGET=event_bus_test
VPATH=../include
.PHONY=test

$(TARGET) : event_bus_test.cpp event_bus.hpp
	$(CXX) -I../include -std=c++11 -g -O0 -o $@ $< -pthread

clean:
	rm -f $(TARGET)

test: $(TARGET)
	./$(TARGET)
