### Notes

+ seccomp:unconfined allows to fork processes.

### Found Limitations

+ L1. Only works for resuming stopped containers, and it has to be the same container. (Removing -> Starting -> Stoping -> Resuming from a checkpoint from the same instance of another container does not work)

### Examples:

1. Counter

2. JS Web Counter:
```
Error response from daemon: Cannot checkpoint container js-looper: runc did not terminate sucessfully: criu failed: type NOTIFY errno 0 path= /run/containerd/io.containerd.runtime.v1.linux/moby/80800f321ecac949445cdf621ecf47c9ff902558014a32a5c5ead9e23618a08f/criu-dump.log: unknown
```
+ It looks as though it might be a kernel error (mine being too new): https://github.com/checkpoint-restore/criu/issues/860
+ It also fails if I run a webserver in alpine

### To-Do

1. Get the simple server example to work
2. Keep running option
3. Be able to start from a different container
