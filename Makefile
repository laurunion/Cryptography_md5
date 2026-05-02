CXX = g++
CXXFLAGS = -Wall -std=c++11

TARGET = md5_demo
SRC = main.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)