# ---------------------------------------------------------------------------- #
#                                  VARIABLES                                   #
# ---------------------------------------------------------------------------- #

NAME			:=	webserv

# ---------------------------------- PROGRAM --------------------------------- #

SRC				+=	main.cpp

# --------------------------------- COMMANDS --------------------------------- #

CPP				:=	c++
RM				:=	/bin/rm -rf
MKDIR			:=	mkdir -p

# ----------------------------------- PATHS ---------------------------------- #

INCLUDES		:=	includes/

SRCS_DIR		:=	srcs/
SRCS			:=	$(addprefix $(SRCS_DIR), $(SRC))

OBJS_DIR		:=	objs/
OBJS			:=	$(addprefix $(OBJS_DIR), $(SRC:.cpp=.o))

DEPS			:=	$(OBJS:.o=.d)

# ----------------------------------- FLAGS ---------------------------------- #

CPPFLAGS		+=	-std=c++98
CPPFLAGS		+=	-Wall
CPPFLAGS		+=	-Wextra
CPPFLAGS		+=	-Werror
CPPFLAGS		+=	-Wconversion
CPPFLAGS		+=	-MMD
CPPFLAGS		+=	-MP

DEBUGFLAGS		+=	-g3

# ---------------------------------- STYLES ---------------------------------- #

RED				:=	"\033[0;31m\033[1m"
GREEN			:=	"\033[0;32m\033[1m"
BLUE			:=	"\033[0;34m\033[1m"
PURPLE			:=	"\033[0;35m\033[1m"
YELLOW			:=	"\033[1;33m\033[1m"
WHITE			:=	"\033[0;37m\033[1m"
NO_STYLE		:=	"\033[0m"

# ----------------------------------- ECHO ----------------------------------- #

ECHO			:=	echo

DELETION		:=	$(RED)Deletion$(NO_STYLE) of
CREATION		:=	$(GREEN)Creation$(NO_STYLE) of
TO_UPPER		:=	tr '[:lower:]' '[:upper:]'
PROJECT			:=	$(shell $(ECHO) $(NAME) | $(TO_UPPER))
OBJ_FILES		:=	$(BLUE)object files$(NO_STYLE)
DEPS_FILES		:=	$(BLUE)dependency files$(NO_STYLE)
AND				:=	and
COMPLETE		:=	complete
BONUS_FILES		:=	bonus object files
PROJECT_NAME	:=	$(WHITE)$(PROJECT)$(NO_STYLE)
NAME_FILE		:=	$(BLUE)$(NAME)$(NO_STYLE)
NORM_MSG		:=	$(GREEN)Norminette on $(PROJECT_NAME)$(GREEN): OK!$(NO_STYLE)

COMP_OBJS		:=	$(ECHO) $(CREATION) $(PROJECT_NAME) $(OBJ_FILES) $(AND) $(DEPS_FILES) $(COMPLETE)
COMP_NAME		:=	$(ECHO) $(CREATION) $(PROJECT_NAME) $(NAME_FILE) $(COMPLETE)
OK_NORM			:=	$(ECHO) $(NORM_MSG)
COMP_CLEAN		:=	$(ECHO) $(DELETION) $(PROJECT_NAME) $(OBJ_FILES) $(AND) $(DEPS_FILES) $(COMPLETE)
COMP_FCLEAN		:=	$(ECHO) $(DELETION) $(PROJECT_NAME) $(NAME_FILE) $(COMPLETE)

define HELP_MSG
Usage: make [TARGET]
		all         - Build the main program
		debug	    - Activate debug flags
		clean       - Remove object file(s)
		fclean      - Remove object file(s) and binary file(s)
		re          - Rebuild the program
		help        - Display this help message
endef
export HELP_MSG

HELP			:=	@$(ECHO) "$$HELP_MSG"

# ---------------------------------------------------------------------------- #
#                                    RULES                                     #
# ---------------------------------------------------------------------------- #

$(OBJS_DIR)%.o:		$(SRCS_DIR)%.cpp
					@$(MKDIR) $(dir $@)
					@$(CPP) $(CPPFLAGS) -I $(INCLUDES) -c $< -o $@

all:				$(NAME)

$(NAME):			$(OBJS)
					@$(COMP_OBJS)
					@$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)
					@$(COMP_NAME)

debug:
					@$(MAKE) $(MAKE_NO_PRINT) CPPFLAGS+="$(DEBUGFLAGS)" re

clean:
					@$(RM) $(OBJS_DIR)
					@$(COMP_CLEAN)

fclean:				clean
					@$(RM) $(NAME)
					@$(COMP_FCLEAN)

re:					fclean all

help:
					$(HELP)

-include			$(DEPS)

.PHONY:				all debug clean fclean re help
