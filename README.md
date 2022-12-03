# Coding Sketch

This repository contains the C++ implementation of Coding Sketch, a data structure for frequency estimation. We compare different aspects of performance of Coding Sketch and other sketches.



## Project Structure

```
├─include
│  ├─params.h-------------parameter for Coding sketch
│  │
│  ├─Choose_Ske.h---------to choose which sketch to run
│  │
│  ├─BIT_CM_ver2.h--------Ours
│  │
│  └─BIT_CM.h-------------flag version
│  
├─main.cpp----------------to test performance for a sketch
│
├─run.py------------------to test performance for multiple sketches
│  
└─changePara.py-----------to change include/params.h
```



## Installation

We recommend that users run this program on a Linux platform.

Use cmake(version >= 3.16.3) and make(version>= 4.2.1) to build the executable file.

```
cmake .
make
```



## Run

In include/params.h, change line 20(const int mem[6]) to assign the number of counters for the array in each layer.

```
#test multiple sketches
python run.py -b

#test flag version using caida dataset
./main.cpp 2 /share/CAIDA_2018/00.dat 0
```

