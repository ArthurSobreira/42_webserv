#------------------------------------------------------------------------------#
#                                  GENERICS                                    #
#------------------------------------------------------------------------------#

.DEFAULT_GOAL: all
.DELETE_ON_ERROR:
.PHONY: all clean fclean re tests

#------------------------------------------------------------------------------#
#                                VARIABLES                                     #
#------------------------------------------------------------------------------#

CC		=	c++
CFLAGS	=	-Wall -Werror -Wextra -std=c++98 -g3 -I includes/
RM		=	rm -rf

NAME		=	webserv
BUILD		=	./build
LOG			=	./logs
CONFIG_PATH = 	src/Config/
RESP_PATH	=	src/Response/
SERVER_PATH =	src/Server/
UTILS_PATH	=	src/Utils/

SRCS		=	$(CONFIG_PATH)Config.cpp \
				$(CONFIG_PATH)ConfigExtract.cpp \
				$(RESP_PATH)CGIResponse.cpp \
				$(RESP_PATH)DeleteResponse.cpp \
				$(RESP_PATH)GetResponse.cpp \
				$(RESP_PATH)PostResponse.cpp \
				$(SERVER_PATH)EpoolManager.cpp \
				$(SERVER_PATH)Fds.cpp \
				$(SERVER_PATH)Server.cpp \
				$(SERVER_PATH)ServerManager.cpp \
				$(UTILS_PATH)CGIUtils.cpp \
				$(UTILS_PATH)ConfigUtils.cpp \
				$(UTILS_PATH)Logger.cpp \
				$(UTILS_PATH)Utils.cpp \
				src/Request.cpp \
				src/Response.cpp \
				src/main.cpp

OBJS		=	$(addprefix $(BUILD)/, $(patsubst src/%, %, $(SRCS:.cpp=.o)))
INC			=	$(wildcard include/*.hpp)

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
	@echo $(GREEN)[Compiling]$(LIMITER) $(WHITE_U)$(notdir $<)...$(LIMITER)
	$(CC) $(CFLAGS) -c $< -o $@ -I include/

$(BUILD):
	@mkdir -p $(BUILD)
	@mkdir -p $(BUILD)/Config/
	@mkdir -p $(BUILD)/Response/
	@mkdir -p $(BUILD)/Server/
	@mkdir -p $(BUILD)/Utils/
	@mkdir -p $(LOG)

clean:
	@echo $(RED)[Cleaning object files...]$(LIMITER)
	@if [ -d $(BUILD) ]; then $(RM) $(BUILD); fi

fclean: clean
	@echo $(RED)[Cleaning executables...]$(LIMITER)
	@echo $(RED)[Cleaning logs files...]$(LIMITER)
	@$(RM) $(NAME)
	@$(RM) $(LOG)

re: fclean all

run: all
	@echo $(CYAN)[Running $(NAME) executable...]$(LIMITER)
	@echo $(GREEN)[Server is running...]$(LIMITER)
	@./$(NAME) config/testLocation.conf

valgrind: all
	@echo $(CYAN)[Running valgrind...]$(LIMITER)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
	./$(NAME) config/testLocation.conf
