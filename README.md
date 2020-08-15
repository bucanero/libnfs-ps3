# libNFS libraries (PS3)

This library port provides NFS (Network File System) access to the PlayStation 3 using the [PSL1GHT SDK](https://github.com/ps3dev/PSL1GHT/).

## libnfs

LIBNFS is a client library for accessing NFS shares over a network.

LIBNFS offers three different APIs, for different use :
- RAW: A fully async low level RPC library for NFS protocols
This API is described in `include/libnfs-raw.h`
it offers a fully async interface to raw XDR encoded blobs.
This API provides very flexible and precise control of the RPC issued.

- NFS ASYNC: A fully asynchronous library for high level vfs functions
This API is described by the *_async() functions in `include/libnfs.h`.
This API provides a fully async access to posix vfs like functions such as
stat(), read(), ...

- NFS SYNC: A synchronous library for high level vfs functions
This API is described by the *_sync() functions in `include/libnfs.h`.
This API provides access to posix vfs like functions such as
stat(), read(), ...

### Build/Install

Download `libnfs` with:
```
git clone https://github.com/sahlberg/libnfs
```

Build the `libnfs` library with: 
```
make -f ps3_ppu/Makefile.PS3_PPU
```

Install the library to your PSL1GHT setup with:
```
make -f ps3_ppu/Makefile.PS3_PPU install
```

## liborbisNfs

`liborbisNfs` is a homebrew userland library based in `libnfs` and it's part of [LIBORBIS](https://github.com/orbisdev/liborbis).

You must use only IP addresses, name resolution is not implemented.
```c
orbisNfsInit("nfs://10.0.0.1/path/of/your/export");
```

### Build/Install

Build the `liborbisNfs` library with: 
```
make
```

Install the library to your PSL1GHT setup with:
```
make install
```

## Credits

 - [sahlberg](https://github.com/sahlberg): [libnfs](https://github.com/sahlberg/libnfs)
 - [orbisdev](https://github.com/orbisdev): [liborbisNfs](https://github.com/orbisdev/liborbis/tree/master/liborbisNfs)

## Server setup

This section explains how to setup a NFS server on your network.

### Linux/Ubuntu

1. Install the necessary packages and create the shared directory:

   ```bash
   sudo apt update && sudo apt install nfs-kernel-server
   # create the shared dir
   sudo mkdir -p ~/NFS/hostapp
   sudo chown nobody:nogroup ~/NFS/hostapp
   sudo chmod -R 777 ~/NFS/hostapp
   ```

2. Configure the NFS exports:

   ```bash
   sudo vim /etc/exports
   # we'll add a line at the bottom of the file following this stucture:
   # [NFS_SHARED_DIR_PATH] *(sync,rw,no_subtree_check,insecure,fsid=0)
   # instead of * you can just allow your PS4 IP to get more security
   # full example: /home/alfa/NFS/hostapp *(sync,rw,no_subtree_check,insecure,fsid=0)
   # remember to don't use the ~ for the path as the service is executed with root permissions!
   /home/alfa/NFS/hostapp *(sync,rw,no_subtree_check,insecure,fsid=0)
   ```

3. Refresh the configuration and start the NFS server:

   ```bash
   sudo exportfs -arvf
   # if everything was okay, you should get an output like this: exporting *:/home/alfa/NFS/hostapp
   # and we restart the server
   sudo systemctl restart nfs-kernel-server
   ```

4. Remember to configure your firewalls to allow NFS server traffic.

### macOS

In this example, we'll make the `/Users/MyShare` folder on the Mac available to other computers on the network.

1. Open the Terminal.

2. Type the following command to edit the `/etc/exports` file:
   ```bash
   sudo nano /etc/exports 
   ```
 
3. Add a line to file similar to the following example (change the IP address and mask to the one used by your network):
   ```bash
   /Users/MyShare -network 192.168.0.0 -mask 255.255.0.0 
   ```
 
   - Press <kbd>CTRL</kbd>+<kbd>O</kbd> and then Enter to save the file.
   - Press <kbd>CTRL</kbd>+<kbd>X</kbd> to exit the text editor.
  
4. Type the following command to enable `nfsd`:
   ```bash
   sudo nfsd enable
   ``` 

5. Type the following command to test that the sharing is properly enabled:
   ```bash
   showmount -e
   ``` 
   This command output should be similar to the following:
   ```
   Exports list on localhost:
   /Users/MyShare                      192.168.0.0
   ```

Now the app using libNFS on your network should be able to mount the `/Users/MyShare` directory on the Mac.

## Documentation

 - [libNFS Readme](./readme_libnfs.txt)

## Sample app

You can find a sample PSL1GHT app using the library [here](./example).

A PS4 example on liborbisNfs is [available here](https://github.com/orbisdev/samples/tree/master/samplenfs).

## License

`libnfs-ps3` is released under the [LGPL 2.1 License](./LICENSE).
