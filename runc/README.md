### Instructions to run containers using runC

1. Create a new directory for the container
`mdkir ./my-container && cd my-container`

2. Create a root filesystem
`mkdir rootfs`

3. Export the docker image into the root filesystem
`docker export $(docker create <image-tag>) | tar -C rootfs -xvf -`

4. Create a spec file
`runc spec`

5. To run rootless containers we must modify a flag in the kernel:
`cat /proc/sys/kernel/unprivileged_userns_clone`
must be set to 1.
```
su -
echo 1 > /proc/sys/kernel/unprivileged_userns_clone
exit
```

6. To set up a network namespace we use [netns](https://github.com/genuinetools/netns)

### Specific Examples

1. [Redis Server](https://github.com/BU-NU-CLOUD-F19/Interoperable_Container_Runtime/wiki/Configuring-network-with-runc:-redis-container): first generate the root filesystem using the command defined below with `redis` as `<image-tag>`.
```
    cd ~/criu-demos/runc/redis && sudo runc run -d eureka &> /dev/null < /dev/null
    sudo netns ls
    redis-cli -h <IP>
    redis> SET mykey "hello"
    sudo runc checkpoint eureka
    //observe that the container has disappeared, (not stopped) hence allowing for transparent migration
    ther than havng to deal with container names around (docker)
    sudo runc restore -d eureka &> /dev/null < /dev/null
    redis-cli -h <IP> // note that it is a new one
    GET mykey
```
