### CRIU + Docker Test

To run the test first install CRIU and Docker. Then build the image here included as follows:
```
docker build -t criu-looper .
```

Launch the image:
```
docker run -d --name looper --security-opt seccomp:unconfined criu-looper:latest
```

You can check the logs of the current image using:
```
docker logs looper
```

Check the current container status:
```
docker ps -a
```

Checkpoint the looper application:
```
docker checkpoint create looper looper-cp1
```

If we want to keep the container running we can use the ``--leave-running`` flag:
```
docker checkpoint create --leave-running looper cp1
```

Check again the container status (should be exitted):
```
docker ps -a
```

Restore the container from the stored checkpoint (note that, unless stated otherwise, all these checkpoints will be mantained by docker):
```
docker start --checkpoint looper-cp1 looper
```

Print the logs again and it should be at the point we froze the execution.
