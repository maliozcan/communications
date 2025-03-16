#!/usr/bin/bash

# Resource: https://clang.llvm.org/extra/clang-tidy/

FILE=compile_commands.json
if [ ! -f $FILE ]; then
    echo "$FILE does not exist. Creating ..."
    make clean
    bear -- make
    echo $FILE created.
    echo Running clang-tidy with $FILE
fi

GLOBIGNORE=third_party/*
clang-tidy --checks='clang-analyzer*, clang-diagnostic*, modernize*, readability*, cppcoreguidelines*, cert*, misc*, performance*, portability*' -p $FILE */*h */*hpp */*cpp
# clang-tidy --checks=* -p $FILE */*h */*hpp */*cpp