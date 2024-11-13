#!/usr/bin/env bash

IN_FILE=$1
OUT_DIR=$2
OUT_NAME=$3

riscv64-unknown-elf-as -o0 -o $OUT_DIR/$OUT_NAME.o $IN_FILE
riscv64-unknown-elf-ld -o $OUT_DIR/$OUT_NAME.elf $OUT_DIR/$OUT_NAME.o