#!/bin/bash

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RESET='\033[0m'

# Function to replace keywords in files within a directory
replace_keywords() {
    local dir="$1"
    local old_keywords=("rhodeus" "Rhodeus" "RHODEUS")
    local new_keywords=("newprojectname" "NewProjectName" "NEWPROJECTNAME")

    # Loop through each file in the directory
    for file in "$dir"/*
    do
        # Check if the file is a regular file
        if [ -f "$file" ]; then

            # If file is current file then skip
            if [ "$file" == "./rename.sh" ]; then
                continue
            fi

            echo -e "${BLUE}Processing file: $file${RESET}"
            # Loop through each old keyword and its corresponding replacement
            for ((i=0; i<${#old_keywords[@]}; i++))
            do
                # Check if the file contains the old keyword
                if grep -q "${old_keywords[$i]}" "$file"; then
                    # Print the line number that contains the old keyword
                    grep -n "${old_keywords[$i]}" "$file"
                fi

                # Perform the replacement using sed
                sed -i "s/${old_keywords[$i]}/${new_keywords[$i]}/g" "$file"
            done
        elif [ -d "$file" ]; then
            # If the file is a directory, recursively call replace_keywords
            replace_keywords "$file"
        fi
    done
}

replace_keywords "."
