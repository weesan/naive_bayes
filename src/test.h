#ifndef TEST_H
#define TEST_H

#include <string>
#include "thread/src/thread.h"

using namespace std;

class NaiveBayes;

class Test : public ThreadPool {
private:
    NaiveBayes &_naive_bayes;
    unsigned int _correct;
    
public:
    Test(NaiveBayes &naive_bayes,
         const string &test_file,
         const string &label_field,
         int parallel, int best_matched = 1);
    NaiveBayes &naive_bayes(void) const {
        return _naive_bayes;
    }
    void incCorrect(void) {
        _correct++;
    }
};

#endif // TEST_H
