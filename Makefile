#------------------------------------------------------------------------------#
#                                  GENERICS                                    #
#------------------------------------------------------------------------------#

DEFAULT_GOAL: all
.DELETE_ON_ERROR: $(NAME)
.PHONY: all bonus clean fclean re

#------------------------------------------------------------------------------#
#                                VARIABLES                                     #
#------------------------------------------------------------------------------#

CC		=	c++
CFLAGS	=	-Wall -Werror -Wextra -std=c++98 -g3 -I includes/
RM		=	rm -rf

NAME	=	webserv
BUILD	=	./build
SRCS	=	src/main.cpp  src/cgi_handler.cpp  src/response.cpp src/server.cpp src/request.cpp   src/utils.cpp src/HttpError.cpp src/Logger.cpp src/getters
OBJS	=	$(addprefix $(BUILD)/, $(notdir $(SRCS:.cpp=.o)))
INC		=	$(wildcard include/*.hpp)

GREEN	=	"\033[32;1m"
RED		=	"\033[31;1m"
CYAN	=	"\033[36;1;3m"
WHITE_U	=	"\033[37;1;4m"
LIMITER =	"\033[0m"

#------------------------------------------------------------------------------#
#                                 TARGETS                                      #
#------------------------------------------------------------------------------#

all: $(BUILD) $(NAME)

$(NAME): $(OBJS)
	@echo $(CYAN)[$(NAME) executable created successfully...]$(LIMITER)
	@$(CC) $(CFLAGS) $(OBJS) -o $@ -I include/

$(BUILD)/%.o: src/%.cpp $(INC)
	@echo $(GREEN)[Compiling]$(LIMITER) $(WHITE_U)$(notdir $(<))...$(LIMITER)
	$(CC) $(CFLAGS) -c $< -o $@ -I include/

$(BUILD):
	@mkdir -p $(BUILD)

clean:
	@echo $(RED)[Cleaning object files...]$(LIMITER)
	@$(RM) $(BUILD)

fclean: clean
	@echo $(RED)[Cleaning $(NAME) executable...]$(LIMITER)
	@$(RM) $(NAME)

re: fclean all

run: all
	./$(NAME) config/server.conf