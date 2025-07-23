#!/bin/sh

read -p 'UID: '

tar czvf "$REPLY.tar.gz" \
	Makefile \
	proc_count.c \
	README.md \
	test_lab0.py
