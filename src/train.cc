#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "train.h"
#include "naive_bayes.h"

//#define SINGLE_THREADED
#define TRAIN_MAX_QUEUE_SIZE    1000

using namespace std;

static void train_helper (Train &train,
                          const string &line,
                          const string &label_field)
{
    //cout << line << endl;
    string label;
    vector<string> fields;
    NaiveBayes &naive_bayes = train.naive_bayes();

    switch (naive_bayes.input_format()) {
    case NaiveBayes::JSON: {
        Json json = Json::parse(line).flatten();
        //cout << json.dump(4) << endl;
        label = json[label_field];
        naive_bayes.extract_fields(json, fields);
        break;
    }
    case NaiveBayes::CSV: {
        vector<string> tokens;
        boost::split(tokens, line, boost::is_any_of(","));
        label = tokens[atoi(label_field.c_str())];
        naive_bayes.extract_fields(tokens, fields);
        break;
    }
    }
        
    for (auto itr = fields.begin(); itr != fields.end(); ++itr) {
        //cout << itr.key() << ": " << itr.value() << endl;
        //cout << *itr << endl;
        naive_bayes.inc_label_count(label, *itr);
        //naive_bayes.inc_event_count(*itr, label);
    }
}

class TrainTask : public Task {
private:
    Train &_train;
    string _line;
    string _label_field;

public:
    TrainTask(Train &train, const string &line, const string &label_field) :
        _train(train),
        _line(line),
        _label_field(label_field) {
    }
    void run(void) {
        train_helper(_train, _line, _label_field);
    }
};

Train::Train (NaiveBayes &naive_bayes,
              const string &train_file,
              const string &label_field,
              int parallel) :
    ThreadPool(parallel),
    _naive_bayes(naive_bayes) {
    int total = 0;
    string line;
    ifstream ifs(train_file);

    while (getline(ifs, line)) {
        total++;

        while (queueSize() >= TRAIN_MAX_QUEUE_SIZE) {
            sleep(0.5);
        }
        
#ifdef SINGLE_THREADED
        train_helper(*this, line, label_field);
#else        
        addTask(new TrainTask(*this, line, label_field));
#endif        
    }

    // Signal the thread pool we are done with populating the jobs.
    done();

    // Wait for the thread pool to finish all the tasks.
    wait();

    // Post process the total.
    _naive_bayes.post_process();
}

