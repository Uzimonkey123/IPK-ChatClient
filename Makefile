CXX = g++
CXXFLAGS = -std=gnu++20 -Wall -Wextra -Werror -pedantic -g

TARGET = ipk24chat-client

SRCS = parser.cpp lastMessage.cpp strings.cpp fsm.cpp udp.cpp tcp.cpp validator.cpp formater.cpp commands.cpp main.cpp
HEADERS = $(SRCS:.cpp=.hpp)

OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	$(RM) $(TARGET) $(OBJS)