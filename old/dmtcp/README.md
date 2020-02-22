### DMTCP CR Test

In this folder we include code to perform a CR of a simple application using [DMTCP: Distributed MultiThreaded CheckPointing](https://github.com/dmtcp/dmtcp).

To replicate follow these steps:

0. Clone this repo and compile the code running `make`.

1. Run the coordinator in window:
```
dmtcp_coordinator
```

2. In a **separate** terminal, run the compiled binary. If compiling your own source code note that you must include and link to the DMTCP sources. Note that, starting from recent versions of DMTCP, you must specify the host and port of the coordinator, this information can be read from the coordinator window.
```
dmtcp_launch -h 127.0.0.1 -p 7779 counter
```

3. At any point press `c<RETURN>` on the coordinator to checkpoint the running application and `k<RETURN>` to kill it.

4. Once killed, recall the `.dmtcp` file created in the running directory, you can restart the binary runing:
```
dmtcp_restart -h 127.0.0.1 -p 7779 *.dmtcp
```
