#include "Typedef.h"

const std::string retract = "    ";

const std::string HELP =
    "HELP Command Description:\n"
    "\n"
    "    - RUN: Begins execution of the program starting from the lowest-numbered line. Program flow is sequential unless altered by GOTO and IF statements. Execution concludes upon reaching the END statement or after the final line of the program.\n"
    "\n"
    "    - LOAD: Loads a program from a file. A dialog will prompt you to select the file. Once loaded, the program's statements and commands are executed as if they were inputted directly.\n"
    "\n"
    "    - CLEAR: Clears the current program from memory, allowing you to start writing a new program.\n"
    "\n"
    "    - HELP: Displays this help message, providing information about the commands available in this BASIC interpreter.\n"
    "\n"
    "    - QUIT: Exits the BASIC interpreter. Type QUIT to terminate the session.\n"
    "\n"
    "    Note: The LIST command has been deprecated. All entered code is displayed in real time, eliminating the need for a separate list command.\n";
