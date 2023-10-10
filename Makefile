NAME = webserv
CXXFLAGS = -Wall -Wextra -Werror -MMD -std=c++98
LDFLAGS = 

INCLUDES = -I ./includes \
			-I ./includes/core \
			-I ./includes/core/config \
			-I ./includes/event \
			-I ./includes/http \
			-I ./includes/stream
OBJ_DIR = objs

SRCS := $(wildcard srcs/core/*.cpp) \
		$(wildcard srcs/core/config/*.cpp) \
		$(wildcard srcs/http/*.cpp) \
		$(wildcard srcs/*.cpp) \
		$(wildcard srcs/event/*.cpp)
# $(wildcard srcs/event/modules/*.cpp)
# $(wildcard srcs/stream/*.cpp)
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
	$(CXX) $(INCLUDES) -c $< -o $@

clean :
	rm -rf $(OBJ_DIR)
	rm -rf ./$(NAME).dSYM

fclean : clean
	rm -f webserv
	rm -f leaks.txt

re : 
	make fclean
	make all -j4

leaks :
	valgrind --leak-check=full --show-leak-kinds=all --log-file=leaks.txt $(PWD)/$(NAME) $(PWD)/configs/example.conf

.PHONY : all clean fclean re leaks

-include $(DEPS)