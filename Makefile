#
# Makefile 
# hashtable
#
# Created by Victor J. Roemer on 3/16/12.
# Copyright (c) 2012 Victor J. Roemer. All rights reserved.
#

CC=gcc
CFLAGS=-O3 -g -std=c99 -Werror -Wall -Wextra -Wformat -Wformat-security -pedantic

FILES=src/digest.c src/digest.h src/hashtable.c src/hashtable.h src/main.c

all: ${FILES}
	${CC} ${CFLAGS} $^ -o test

clean: test
	rm test
