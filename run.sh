#!/bin/bash
cd build && make clean && make && cd .. && cd bin && ./test && cd ..
