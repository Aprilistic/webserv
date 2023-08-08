NAME = webserv
CXXFLAGS = -Wall -Wextra -Werror -MMD -std=c++98
LDFLAGS = 

INCLUDES = -I src/core \
		-I src/event/modules \
		-I src/event \
		-I src/http/modules \
		-I src/http \
		-I src/stream
OBJ_DIR = objs

SRCS := $(wildcard src/core/*.cpp) \
		$(wildcard src/event/modules/*.cpp) \
		$(wildcard src/event/*.cpp) \
		$(wildcard src/http/modules/*.cpp) \
		$(wildcard src/http/*.cpp) \
		$(wildcard src/stream/*.cpp)
SRCS_DIR := $(dir $(SRCS))

OBJS := $(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:.cpp=.o)))


DEPS = $(OBJS:.o=.d)

vpath %.cpp $(SRCS_DIR)

all : $(OBJ_DIR) $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJ_DIR) :
	mkdir $@

$(OBJ_DIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean :
	rm -rf $(OBJ_DIR)

fclean : clean
	rm -f webserv

re : 
	make fclean
	make all

.PHONY : all clean fclean re

-include $(DEPS)