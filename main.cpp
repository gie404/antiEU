//
// Created by gie on 04.08.18.
//

#include <string>
#include <iostream>

#include "antiEu.h"

int main(int argc, char *argv[])
{
    using namespace std;

    long int depth = 4;

    for (int i = 1; i < argc; i++) {
        string opt{argv[i]};
        if (opt == "-d") {
            if (i + 1 >= argc) {
                cerr << "no argument for option " << opt << endl;
                return -1;
            }

            i++;
            char* end;
            string arg{argv[i]};
            long int newDepth = strtol(arg.c_str(), &end, 10);
            if (*end != '\0'){
                cerr << "can not parse argument '" << arg << "' for option " << opt << endl;
                return -1;
            }
            depth = newDepth;
        }
        else {
            cerr << "invalid option " << opt << endl;
            return -1;
        }
    }

    cout << "start with options:" << endl;
    cout << "depth " << depth << endl;

    return antiEu(depth);
}
