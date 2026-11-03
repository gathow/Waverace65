#!/bin/bash

# Change to the project directory
cd /home/georgeathow/Documents/Waverace65/

# Compile the program and capture errors
g++ main.cpp src/glad.c -I include -o main -lglfw -lGL 2> /home/georgeathow/Documents/Waverace65/compile_error.log

# Check if the compilation was successful
if [ $? -eq 0 ]; then
    # Run the compiled program
    ./main
else
    echo "Compilation failed."
    # Display the error message
    cat /home/georgeathow/Documents/Waverace65/compile_error.log
fi