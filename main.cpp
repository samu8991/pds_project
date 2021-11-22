#include<iostream>

#include "menu.h"

int 
main(){
    po::options_description cmd_options("If the program is executed without "
                                        "an input file, an interactive "
                                        "version will run.\n"
                                        "It is also possibile to run the program "
                                        "without explicit indication, using the "
                                        "following order: input, internal_representation, algorithm, threads[, trials].\n"
                                        "e.g. GraphColoring INPUT_PATH 0 1 2 2, GraphColoring INPUT_PATH 1 1 1 2 5\n"
                                        "Available options");
    return EXIT_SUCCESS;
}
