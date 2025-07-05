# A Kernel Seedling
The following is a kernel module which creates a file `/proc/count` containing
the number of active processes.

## Building
```shell
make
```

## Running
Activate the module with:
```shell
sudo insmod proc_count.ko
```

You can now find a file `/proc/count` with a number representing the number of processes.

## Cleaning Up
```shell
make clean
```

## Testing
The module is tested by running the following in the source directory:
```python
python -m unittest
```

It will run three tests, outputing OK or any errors it finds with making the
module, inserting it to the kernel, testing its functionality, and removing the
module.

A successful test's output looks like the following:
```
...
-------------------------------------------------
Ran 3 tests in 0.925s

OK
```

This module was tested on version 5.14.8 of the Linux kernel. The specific
version tested was
`Linux 5.14.8-arch-1-1 #1 SMP PREEMPT Sun, 26 Sep 2021 19:36:15 +0000`
as found using `uname -r -s -v`.
