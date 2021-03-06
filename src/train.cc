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

static void train_helper (Train &train, const string &line)
{
    //cout << line << endl;
    string label;
    vector<string> fields;
    NaiveBayes &naive_bayes = train.naive_bayes();
    const string &label_field = naive_bayes.label_field();

    switch (naive_bayes.input_format()) {
    case NaiveBayes::JSON: {
        Json json = Json::parse(line).flatten();
        //cout << json.dump(4) << endl;
        label = json[label_field];
        naive_bayes.field_table().extract(json, fields);
        break;
    }
    case NaiveBayes::CSV: {
        vector<string> tokens;
        boost::split(tokens, line, boost::is_any_of(","));
        label = tokens[atoi(label_field.c_str())];
        naive_bayes.field_table().extract(tokens, fields);
        break;
    }
    }
        
    for (auto itr = fields.begin(); itr != fields.end(); ++itr) {
        //cout << *itr << endl;
        naive_bayes.inc_label_count(label, *itr);
        //naive_bayes.inc_event_count(*itr, label);
    }
}

class TrainTask : public Task {
private:
    Train &_train;
    string _line;

public:
    TrainTask(Train &train, const string &line) :
        _train(train),
        _line(line) {
    }
    void run(void) {
        train_helper(_train, _line);
    }
};

Train::Train (NaiveBayes &naive_bayes, const vector<string> &train_files) :
    ThreadPool(naive_bayes.parallel()),
    _naive_bayes(naive_bayes) {    
    int total = 0;

    for (int i = 0; i < train_files.size(); i++) {
        string line;
        ifstream ifs(train_files[i]);
        if (!ifs) {
            cerr << "Failed to open: " << train_files[i] << endl;
            exit(-1);
        }
        
        while (getline(ifs, line)) {
            total++;

            while (queueSize() >= TRAIN_MAX_QUEUE_SIZE) {
                sleep(0.5);
            }
        
#ifdef SINGLE_THREADED
            train_helper(*this, line);
#else
            addTask(new TrainTask(*this, line));
#endif
        }
    }

    // Signal the thread pool we are done with populating the jobs.
    done();

    // Wait for the thread pool to finish all the tasks.
    wait();

    // Post process the total.
    _naive_bayes.post_process();
}
