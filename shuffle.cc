#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main (int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s buckets file\n", argv[0]);
        exit(-1);
    }

    int buckets = atoi(argv[1]);
    const char *file = argv[2];

    vector<ofstream> ofs;
    for (int i = 0; i < buckets; i++) {
        char fs[256];
        snprintf(fs, sizeof(fs), "%d", i);
        ofs.push_back(ofstream(fs));
    }
    
    string line;
    ifstream ifs(file);
    
    while (getline(ifs, line)) {
        int i = rand() % buckets;
        ofs[i] << line << endl;
    }
    
    return 0;
}
