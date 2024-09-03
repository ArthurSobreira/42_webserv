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

NAME	=	webserv
TESTNAME =	testes  # Nome do executável de teste
BUILD	=	./build
SRCS	=	src/cgi_handler.cpp src/response.cpp \
			src/server.cpp src/request.cpp src/utils.cpp \
			src/HttpError.cpp src/Logger.cpp src/getters.cpp \
			src/main.cpp  # Inclui o main para o binário principal
TEST_SRCS = test/main.cpp $(filter-out src/main.cpp, $(SRCS))  # Exclui src/main.cpp e usa test/main.cpp
OBJS	=	$(addprefix $(BUILD)/, $(notdir $(SRCS:.cpp=.o)))
OBJSTESTS =	$(addprefix $(BUILD)/, $(notdir $(TEST_SRCS:.cpp=.o)))
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

$(TESTNAME): $(OBJSTESTS)
	@echo $(CYAN)[$(TESTNAME) test executable created successfully...]$(LIMITER)
	@$(CC) $(CFLAGS) $(OBJSTESTS) -o $@ -I include/

$(BUILD)/%.o: src/%.cpp $(INC)
	@echo $(GREEN)[Compiling]$(LIMITER) $(WHITE_U)$(notdir $<)...$(LIMITER)
	$(CC) $(CFLAGS) -c $< -o $@ -I include/

$(BUILD)/%.o: test/%.cpp $(INC)
	@echo $(GREEN)[Compiling Test]$(LIMITER) $(WHITE_U)$(notdir $<)...$(LIMITER)
	$(CC) $(CFLAGS) -c $< -o $@ -I include/

$(BUILD):
	@mkdir -p $(BUILD)

clean:
	@echo $(RED)[Cleaning object files...]$(LIMITER)
	@find $(BUILD) -type f -name '*.o' -delete  # Remove apenas os arquivos de objeto

fclean: clean
	@echo $(RED)[Cleaning executables...]$(LIMITER)
	@$(RM) $(NAME) $(TESTNAME)

re: fclean all

run: all
	@echo $(CYAN)[Running $(NAME) executable...]$(LIMITER)
	@$(RM) -rf ./logs/server.log
	@echo $(GREEN)[Server is running...]$(LIMITER)
	./$(NAME) config/server.conf

tests: $(BUILD) $(TESTNAME)
	@echo $(CYAN)[Running tests...]$(LIMITER)
	./$(TESTNAME) config/server.conf
