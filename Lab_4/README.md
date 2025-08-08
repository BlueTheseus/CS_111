UID: 805796976
# Hash Hash Hash
TODO introduction

## Building
The project is simply built with make while in the project's directory:
```shell
# make
```

## Running
Build the project. Then it can be run as follows:
```shell
# ./hash-table-tester [-t THREADS] [-s SIZE]
```
The options available are as follows:
- `THREADS`: the number of threads to allocate to the program. By default, this is `4`.
- `SIZE`: the amount of hash table entries to add per thread. By default, this is `25000`.

## First Implementation
In `hash-table-v1.c`, I initialized a POSIX thread mutex as follows:
```c
pthread_mutex_t v1_lock = PTHREAD_MUTEX_INITIALIZER;
```

Then in the `hash_table_v1_add_entry` function, I treated the entire function as
critical code--the function began with holding a lock and ended with giving up
the lock. This was achieved with the respective `pthread_mutex_lock(&v1_lock);`
and `pthread_mutex_unlock(&v1_lock);` function calls. With this implementation,
a thread would be created to add an entry and immediately hold the lock so no
other thread could do any work adding an entry until the first is finished, thus
acting sequentially.

### Performance
To test performance, run the program like usual and the first implementation's
performance will look similar to the example below:
```shell
# ./hash-table-tester -t 4 -s 25000
Generation: 10,513 usec
Hash table base: 24,129 usec
  - 0 missing
Hash table v1: 54,863 usec /* Time for v1 to complete in microseconds */
  - 0 missing              /* Amount of missing entries in the hash table due to v1 */
Hash table v2: 18,942 usec
  - 0 missing
```
where `Hash table v1` states the amount of time the first implementation took
along with how many entries are missing in the hash table due to the implementation.

Version 1 is a little slower than the base version since it includes the
overhead of creating a new thread in addition to still adding entries sequentially.

## Second Implementation
In `hash-table-v2.c`, I initialized a POSIX thread mutex like with v1 above:
```c
pthread_mutex_t v2_lock = PTHREAD_MUTEX_INITIALIZER;
```

Then in the `hash_table_v2_add_entry` function, each thread could define,
initialize, and operate on its local variables as they are created. A thread
only obtains a lock to insert an element to the hash table and immediately gives
up the lock once it is done. This results in the following being the only block
of critical code in the function:

```c
pthread_mutex_lock(&v2_lock); /* Start critical code */
SLIST_INSERT_HEAD(list_head, list_entry, pointers);
pthread_mutex_unlock(&v2_lock); /* End critical code */
```

Hence each thread is able to act on its local variables in parallel to all the
other threads and is only required to wait once it reaches the critical code
section which must be carefully executed to avoid race conditions.

### Performance
To test performance, run the program like usual and the second implementation's
performance will look similar to the example above, which is shown again here:
```shell
$ ./TEST.sh 8 50000
Generation: 39,934 usec
Hash table base: 596,660 usec
  - 0 missing
Hash table v1: 989,279 usec
  - 0 missing
Hash table v2: 112,934 usec /* Time for v2 to complete in microseconds */
  - 0 missing              /* Amount of missing entries in the hash table due to v2 */
```
where `Hash table v2` states the amount of time the first implementation took
along with how many entries are missing in the hash table due to the implementation.

Version 2 is a lot faster than the base version since although it includes the
overhead of creating and destroying threads, each thread is able to act in parallel
and is only required to wait for a different thread to finish once it is ready
to act on the data in the hash table.

Across ten tests, v2 was shown to be an average of five times faster than the
base implementation when run with 8 threads and a size of 50,000.

## Cleaning up
To clean up the program and unnecessary files, simply run:
```shell
# make clean
```
