#ifndef TEST_H
#define TEST_H

#include "thread.h"

class NaiveBayes;

class Test : public ThreadPool {
private:
    NaiveBayes &_naive_bayes;
    const char *_test_file, *_label_field;
    unsigned int _total, _correct;
    int _best_matched;
    
public:
    Test(NaiveBayes &naive_bayes,
         const char *test_file, const char *label_field,
         int parallel = 1, int best_matched = 1) :
        ThreadPool(parallel),
        _naive_bayes(naive_bayes),
        _test_file(test_file),
        _label_field(label_field),
        _total(0),
        _correct(0),
        _best_matched(best_matched) {
    }
    void run(void);
    void incCorrect(void) {
        _correct++;
    }
};

#endif // TEST_H
