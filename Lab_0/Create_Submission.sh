#!/bin/sh

read -p 'UID: ' UID

tar czvf "$UID.tar.gz" \
	Makefile \
	proc_count.c \
	README.md \
	test_lab0.py
