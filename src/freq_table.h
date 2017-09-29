#ifndef FREQ_TABLE_H
#define FREQ_TABLE_H

#include <unordered_map>
#include <string>

using namespace std;

class FreqTable : public unordered_map<string, unordered_map<string, size_t> > {
private:
    size_t _total;
public:
    FreqTable(void) : _total(0) {
    }
    size_t const total(void) const {
        return _total;
    }
    void computeTotal(void) {
        for (auto i = begin(); i != end(); ++i) {
            size_t total = 0;
            for (auto j = i->second.begin(); j != i->second.end(); ++j) {
                total += j->second;
            }
            (*this)[i->first]["_total"] = total;
            _total += total;
        }
    }
    void dump(void) {
        for (auto i = begin(); i != end(); ++i) {
            cout << i->first << ": ";
            for (auto j = i->second.begin(); j != i->second.end(); ++j) {
                cout << "(" << j->first << ", " << j->second << ") ";
            }
            cout << endl;
        }
    }
};
    
#endif // FREQ_TABLE_H
