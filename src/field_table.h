#ifndef FIELD_TABLE_H
#define FIELD_TABLE_H

#include <unordered_map>

using namespace std;

class FieldTable : public unordered_map<string, bool> {
public:
    FieldTable(const char *field_file);
};

#endif // FIELD_TABLE_H
