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
TESTNAME	=	testes  
BUILD		=	./build
LOG			=	./logs
BUILD_TEST	=	./build_test
SRCS		=	src/Config.cpp src/HttpError.cpp src/listDirectory.cpp \
				src/Logger.cpp src/Request.cpp src/Response.cpp \
				src/Server.cpp src/Utils.cpp src/main.cpp\

TEST_SRCS	=	test/main.cpp $(filter-out src/main.cpp, $(SRCS)) 
OBJS		=	$(addprefix $(BUILD)/, $(notdir $(SRCS:.cpp=.o)))
OBJSTESTS	=	$(addprefix $(BUILD_TEST)/, $(notdir $(TEST_SRCS:.cpp=.o)))
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

$(TESTNAME): $(BUILD_TEST) $(OBJSTESTS)
	@echo $(CYAN)[$(TESTNAME) test executable created successfully...]$(LIMITER)
	@$(CC) $(CFLAGS) $(OBJSTESTS) -o $@ -I include/

$(BUILD)/%.o: src/%.cpp $(INC)
	@echo $(GREEN)[Compiling]$(LIMITER) $(WHITE_U)$(notdir $<)...$(LIMITER)
	$(CC) $(CFLAGS) -c $< -o $@ -I include/

$(BUILD_TEST)/%.o: test/%.cpp $(INC)
	@echo $(GREEN)[Compiling Test]$(LIMITER) $(WHITE_U)$(notdir $<)...$(LIMITER)
	$(CC) $(CFLAGS) -c $< -o $@ -I include/

$(BUILD_TEST)/%.o: src/%.cpp $(INC)
	@echo $(GREEN)[Compiling Test Dependencies]$(LIMITER) $(WHITE_U)$(notdir $<)...$(LIMITER)
	$(CC) $(CFLAGS) -c $< -o $@ -I include/

$(BUILD):
	@mkdir -p $(BUILD)
	@mkdir -p $(LOG)

$(BUILD_TEST):
	@mkdir -p $(BUILD_TEST)

clean:
	@echo $(RED)[Cleaning object files...]$(LIMITER)
	@if [ -d $(BUILD) ]; then $(RM) $(BUILD); fi
	@if [ -d $(BUILD_TEST) ]; then $(RM) $(BUILD_TEST); fi

fclean: clean
	@echo $(RED)[Cleaning executables...]$(LIMITER)
	@echo $(RED)[Cleaning logs files...]$(LIMITER)
	@$(RM) $(NAME) $(TESTNAME)
	@$(RM) $(LOG)

re: fclean all

run: all
	@echo $(CYAN)[Running $(NAME) executable...]$(LIMITER)
	@echo $(GREEN)[Server is running...]$(LIMITER)
	@./$(NAME) config/server.conf

tests: $(TESTNAME)
	@echo $(CYAN)[Running tests...]$(LIMITER)
	./$(TESTNAME) config/server.conf
