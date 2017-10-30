#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include "field_table.h"

FieldTable::FieldTable (const char *field_file)
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
        
        vector<string> tokens;
        boost::split(tokens, line,
                     boost::is_any_of(" \t,"), boost::token_compress_on);

        // Assign the corresponding weight to each field, or 0 if not given.
        (*this)["/" + tokens[0]] =
            tokens.size() == 1 ? 0.0 : atof(tokens[1].c_str());
    }

    // Re-distribute the leftover weights equally to the rest of the fields.
    int zero = 0;
    float total = 0.0;
    
    for (auto itr = begin(); itr != end(); ++itr) {
        float percentage = itr->second;
        if (percentage == 0.0) {
            zero++;
        } else {
            total += percentage;
        }
    }

    float dist_percentage = (1.0 - total) / zero;
    
    for (auto itr = begin(); itr != end(); ++itr) {
        if (itr->second == 0.0) {
            itr->second = dist_percentage;
        }
    }

    //dump();
}

void FieldTable::extract(Json &json, vector<string> &fields)
{
    if (size() == 0) {
        return;
    }

    //cout << json.dump(4) << endl;
    for (auto itr = begin(); itr != end(); ++itr) {
        auto field = json[itr->first];
        if (field.is_null()) {
            continue;
        }
        //cout << '[' << itr->first << "] <==> [" << field << ']' << endl;
        string str_field;
        if (field.is_string()) {
            str_field = field;
        } else if (field.is_number()) {
            str_field = to_string(field.get<int>());
        }

        // Split the string into words.
        vector<string> tokens;
        boost::split(tokens, str_field, boost::is_any_of(" ,"));
        copy(tokens.begin(), tokens.end(), back_inserter(fields));
    }

    /*
    for (int i = 0; i < fields.size(); i++) {
        cout << '[' << fields[i] << "] ";
    }
    cout << endl;
    */
}

void FieldTable::extract(const vector<string> &tokens, vector<string> &fields)
{
    if (size() == 0) {
        return;
    }

    for (auto itr = begin(); itr != end(); ++itr) {
        fields.push_back(tokens[atoi(itr->first.c_str())]);
    }
}

void FieldTable::dump(void)
{
    for (auto itr = begin(); itr != end(); ++itr) {
        cout << itr->first << ": " << itr->second << endl;
    }
}
