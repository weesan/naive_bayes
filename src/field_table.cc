#include <fstream>
#include "field_table.h"

FieldTable::FieldTable(const char *field_file)
{
    if (!field_file) {
        return;
    }

    // Construct the field table from the field file.
    string line;
    ifstream ifs(field_file);
    
    while (getline(ifs, line)) {
        if (line[0] == '#' || line[0] == ';') {
            continue;
        }
        (*this)[line] = true;
    }
}
