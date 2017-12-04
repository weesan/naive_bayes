# Naive Bayes Classifer

## Introduction

This project implements Naive Bayes Classifer in C++.  It utilizes the
[Thread Pool](http://github.com/weesan/thread/) for multi-processing
during training and testing.

## Other Dependencies

It also makes use of [glog](https://github.com/google/glog), [boost
library](http://www.boost.org/) and [Json for Modern C++](https://github.com/nlohmann/json).

## Installation

    $ configure
    $ make
    $ make install

## Execution Examples

### To get help

    $ src/naive_bayes -h

### To train and test data in Json format

    $ src/naive_bayes -J -t train.json -T test.json -l label -f field_map.txt -P8

#### Explanation

    -J: The input file is in Json format.  If is in CSV format, use -C.
    -t: The train file in json format.
    -T: The test file in json format.
    -l: The field name in the json file serves as the label of the entry.
    -f: The field map indicates which fields in the json file will
        be used to train and test.  One field name per each line.
    -P: Parallel processing for the training and testing.

Comments, questions and pull requests are equally welcome.

Thanks,<br>
-WeeSan &lt;<weesan@weesan.com>&gt;
