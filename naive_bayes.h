#ifndef NAIVE_BAYES_H
#define NAIVE_BAYES_H

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;

class WordFreq : public unordered_map<string, size_t> {
private:
    size_t _totalWords;
    
public:
    WordFreq(void) : _totalWords(0) {
    }
    size_t totalWords(void) const {
        return _totalWords;
    }
    size_t countTotal(void) {
        _totalWords = 0;
        for (auto itr = begin(); itr != end(); ++itr) {
            _totalWords += itr->second;
        }
        return _totalWords;
    }
    void dump(void) const {
        for (auto itr = begin(); itr != end(); ++itr) {
            cout << "(" << itr->first << ": " << itr->second << ") ";
        }
    }
};

/*
 * Label1: (word, count), (word, count), ...
 * Label2: (word, count), (word, count), ...
 * ...
 * where word can be any words, and count is the frequent of the word.
 */
class BagOfWords : public unordered_map<string, WordFreq> {
private:
    size_t _totalLabels;

public:
    BagOfWords(void) : _totalLabels(0) {
    }
    size_t totalLabels(void) const {
        return _totalLabels;
    }
    size_t totalUniqueLabels(void) const {
        return size();
    }
    size_t countTotalLabels(void) {
        _totalLabels = 0;
        for (auto itr = begin(); itr != end(); ++itr) {
            _totalLabels += itr->second.countTotal();
        }
        return _totalLabels;
    }
    void dump(void) const {
        for (auto itr = begin(); itr != end(); ++itr) {
            const string &label = itr->first;
            const WordFreq &wordFreq = itr->second;
            cout << "(" << label << ", " << wordFreq.size() << "): ";
            wordFreq.dump();
            cout << endl;
        }
    }
};

class NaiveBayes {
private:
    BagOfWords _bagOfWords;
    
public:
    void train(const char *train_file);
    float probability(const string &c, const string &x);
    void dump(void) {
        _bagOfWords.dump();
    }
};

#endif // NAIVE_BAYES_H
