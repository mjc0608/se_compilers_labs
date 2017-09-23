#!/bin/bash

../a.out $1
gcc -m32 $1.s ../runtime.c
