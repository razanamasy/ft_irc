SRCS	:=	main.cpp		\
			server.cpp		\
			user.cpp		\
			channel.cpp		\
			utils.cpp	\
			utils_commands.cpp	\
			commands.cpp	\
			replies.cpp		\
			message.cpp		\

OBJS	=	${SRCS:.cpp=.o}

DEP		=	${SRCS:.cpp=.d}

FLAGS	:=	-Wall -Wextra -Werror -g -std=c++98

NAME	:=	ircserv

.SUFFIXES: .cpp .o .d

.cpp.d:
		c++ -MM $< -MF ${<:.cpp=.d}

.cpp.o:
		c++ ${FLAGS} -c $< -o ${<:.cpp=.o}

all:	dependencies ${NAME}

clean:
		rm -f ${OBJS}
		rm -f ${DEP}

fclean:		clean
		rm -f ${NAME}

${NAME}: ${OBJS}
		c++ ${FLAGS} -o ${NAME} ${OBJS}

dependencies:
include ${DEP}

re:		fclean all

.PHONY:		all clean fclean re dependencies
