NAME = webserv
CXXFLAGS = -Wall -Wextra -Werror -MMD -std=c++98
LDFLAGS = 

INCLUDES = -I ./includes \
			-I ./includes/utils \
			-I ./includes/core \
			-I ./includes/core/config \
			-I ./includes/event \
			-I ./includes/http \
			-I ./includes/http/CGI \
			-I ./includes/http/request \
			-I ./includes/http/response
OBJ_DIR = objs

TMP_DIR = tmp
COOKIE_DIR = cookie

SRCS := $(wildcard srcs/*.cpp) \
		$(wildcard srcs/utils/*.cpp) \
		$(wildcard srcs/core/*.cpp) \
		$(wildcard srcs/core/config/*.cpp) \
		$(wildcard srcs/http/*.cpp) \
		$(wildcard srcs/http/CGI/*.cpp) \
		$(wildcard srcs/http/response/*.cpp) \
		$(wildcard srcs/http/request/*.cpp) \
		$(wildcard srcs/http/router/*.cpp) \
		$(wildcard srcs/event/*.cpp)

SRCS_DIR := $(dir $(SRCS))

OBJS := $(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:.cpp=.o)))

DEPS = $(OBJS:.o=.d)

vpath %.cpp $(SRCS_DIR)

all : $(OBJ_DIR) $(TMP_DIR) $(COOKIE_DIR) $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJ_DIR) :
	mkdir $@

$(TMP_DIR) :
	mkdir $@

$(COOKIE_DIR) :
	mkdir $@

$(OBJ_DIR)/%.o : %.cpp
	$(CXX) $(INCLUDES) -c $< -o $@

clean :
	rm -rf $(OBJ_DIR)
	rm -rf ./$(NAME).dSYM
	rm -f ./tmp/*

fclean : clean
	rm -rf $(TMP_DIR) $(COOKIE_DIR)
	rm -f webserv
	rm -f leaks.txt
	rm -f ./www/example/post_*

re : 
	make fclean
	make all -j4

leaks :
	valgrind --leak-check=full --show-leak-kinds=all --log-file=leaks.txt $(PWD)/$(NAME) $(PWD)/configs/example.conf

.PHONY : all clean fclean re leaks

-include $(DEPS)
