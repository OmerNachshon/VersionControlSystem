# VersionControlSystem

The goal of this software is to create an archive that maintains the files on the server or the computer of the user and groups using the software. In many cases, version control systems are beneficial to content writers, coders, engineers etc., even in big companies the use of a physical archive is deeply integrated. So, this project is a way to make local version control, so there is a continous backups to the user's files.


## Project's features:
* login and Authorization mechanisms 
* groups compartmentalization
* locking ability, as user asks to checkout files
* graphical user interface 
* Writing unix's commands in c and c++

## Getting started
### Windows

This part demonstrates how to get started on Windows as well as how to use the Docker setup. If you are **unable to run Docker** then you should try installing Linux.

For starters you have a the Docker installed on your computer, and it is running. The first thing to do is to use it for creating a Docker image. We call the image vcom.

```
docker build . -t vcom   
```

The last command may take up to 10 minutes, depending on your own computer. As it is done, you will receive a complete message saying the process is complete.

Now let's run the image that we created in the last process with Docker
```
docker run -t -d --name vcom vcom  
```

To run the image with Docker use the command below
```
docker exec -it vcom bash
```

If everything is right, you should be able to run ``./vcom`` and get help as to how to use the software

### Linux

The following part demonstrates how to get started on Linux without the Docker setup. Linux is an open source software and it is supported by a community of developers  from all over the world.

For start make sure the right packages installed to your Linux distribution. If the distribution is Ubuntu then you need to run the command below:
```
apt-get update  && \
apt-get -y install vim  && \ 
apt install -y gcc make && \
apt-get install -y g++ libstdc++-10-dev && \
apt-get install -y python3.7 
```

Notice that the last installation is python3.7 and it is only required if you have a graphical interface. vim can also be left out if you are using another favorite editor.

In order to install the software user `make` 
```
cd path/to/folder
make
```

Now you should be able to run ``./vcom`` and get help as to how to use the software.
If you do have a graphical interface distribution, you might want to run `python ./run.py` to have a better interface. In that case you need to run the command below:
```
python ./run.py
```


## Examples

### Archive a file
If you are working on a file that may be updated in the future and you need to back it up continously, then everytime you use the command `checkin` there will be a newer revision for that file in the archive. In the project's folder there is a text file called `txt`, you can archive it by using the following command.
```
./vcom checkin txt
```

Notice the output. At the first time you do checkin on a file in the current folder the output should be:
```
creating dir .vcom/_vcom_txt
file is not locked
Revision: 1
```
Later you can use the revision number to restore the revision in the archive. 

### Retrieve a file
If you need to have an older revision of a file retrieved, then you need to use the command `checkout` with the file that you need. If you do not specify the revision required then you will receive the last known revision. For more information about the command `checkout` read the help document.

```
./vcom checkout -r 1 txt
```

You can ask to lock the file with the flag  `-l` and `lock` right after it

```
./vcom checkout -r 1 -l lock txt
```
The lock will prevent other users and yourself for making another checkout, and also the archive will be avoided to changes to the locked file. It will be locked only until you will make checkin to this file.

### Check differences from last revision 
In case you want to know what is the files differences between an existing file you are currretly editing and a file that is in the archive, you need to use the command `diff` with the filename. You can also use the revisio number that the file is compared to. First, open the file you are working at currently and make some changes to it. Use `vim` or one of your favorite editor in order to make the changes:

```
vim txt
```

The changes to the files are checked as you run the following command:
```
./vcom diff -r 1 txt
```

the output will be the the lines added with `+` sign before them, and removed lines will have the `-`.

