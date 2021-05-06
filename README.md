# Lock-Free-Queue
* An implementation of lock free queue in c++.
* The queue is related to the algorithm proposed in the following [paper](https://www.cs.rochester.edu/~scott/papers/1996_PODC_queues.pdf)
* For understanding the progress in various algorithms such as lock free, lock based, wait free, obstruction free--> The following [paper](https://www.cs.tau.ac.il/~shanir/progress.pdf) can be referred.
* We also provide slides to get a better understanding of progress in various algortihms.
* The slides can be found at the following [link](https://docs.google.com/presentation/d/17kUnBNFOWM2lB9ocI5r4x-pJJ-dSYddbSt3wy_9QWb0/edit#slide=id.p).
* These slides are based on the paper mentioned and also on the some sections of the following [book](http://cs.ipm.ac.ir/asoc2016/Resources/Theartofmulticore.pdf).

# Code Files 
* queue.cpp: Our implementation of linked list based lock free queue.
* boostq.cpp: Using the standard boost lock free queue for testing.
* concurrent.h: This is taken from official implementation of moodycamel to run its lock free queue.

