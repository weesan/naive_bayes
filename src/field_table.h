#ifndef FIELD_TABLE_H
#define FIELD_TABLE_H

#include <unordered_map>
#include <vector>
#include <string>
#include "json.hpp"

using namespace std;
using Json = nlohmann::json;

class FieldTable : public unordered_map<string, float> {
public:
    FieldTable(const char *field_file);
    void extract(Json &json, vector<string> &fields);
    void extract(const vector<string> &tokens, vector<string> &fields);
    void dump(void);
};

#endif // FIELD_TABLE_H
