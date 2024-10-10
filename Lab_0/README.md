# A Kernel Seedling
The following is a kernel module which creates a file `/proc/count` containing the number of active processes.

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
```python
python -m unittest
```
TODO: results?

Report which kernel release version you tested your module on
(hint: use `uname`, check for options with `man uname`).
It should match release numbers as seen on https://www.kernel.org/.

```shell
uname -r -s -v
```
returned that the module was tested with the following kernel:
`Linux 5.14.8-arch-1-1 #1 SMP PREEMPT Sun, 26 Sep 2021 19:36:15 +0000`
