#!/bin/sh
g++ -std=c++11 proj.cpp EasyBMP_1.06/EasyBMP.cpp -o proj -lboost_system -lboost_filesystem
./proj test_dir