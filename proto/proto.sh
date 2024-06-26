#!/bin/bash

# Check if a filename is provided as an argument
if [ $# -ne 1 ]; then
    echo "Usage: $0 <proto_file>"
    exit 1
fi

# Assign the argument to the variable
proto_file=$1

# Check if the specified file exists
if [ ! -f "$proto_file" ]; then
    echo "Error: File '$proto_file' not found."
    exit 1
fi

# Compile the .proto file
protoc --c_out=. "$proto_file"

# Extract the base filename (without extension) for further processing
base_filename=$(basename "$proto_file" .proto)

# Define search and replace text
search_text='#include "'"$base_filename"'.pb-c.h"'
replace_text='#include "proto/'"$base_filename"'.pb-c.h"'

# Perform the search and replace using sed
sed -i "s|$search_text|$replace_text|g" "${base_filename}.pb-c.c"

# Move the generated files to appropriate directories
mv "${base_filename}.pb-c.c" src/proto
mv "${base_filename}.pb-c.h" include/proto
