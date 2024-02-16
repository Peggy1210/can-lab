Introduction to Computer Networking - Homework
===

In this repo, we implement socket programming.

## Lab 1

Write a program that obtain all the hyperlinks in a given web page through the HTTP protocol.

Example:
```
Enter the hostname: can.cs.nthu.edu.tw/index.php
socket: Start send HTTP request
socket: Start read the response
socket: Finish read to buffer
======== Hyperlinks ========
index.php
members.php
LAB/
gallery.php
contact.php
http://web.cs.nthu.edu.tw/files/14-1015-143485,r109-1.php?Lang=zh-tw
http://www.nthu.edu.tw
http://web.cs.nthu.edu.tw/bin/home.php
http://www.com.nthu.edu.tw/
http://www.highimpact-seo.co.uk/

There are 10 hyperlinks in http://can/cs.nthu.edu.tw
```

### Environments

* Ubuntu 20.04 LTS

### Run

```
$ gcc -o lab1 lab1.c
$ ./lab1
```

## Lab 2

Write a server program and a client program, where the client can download a video file from the server using stop-and-wait mechanism through a UDP socket.

### Environments

* Ubuntu 20.04 LTS

### Run

* server
    ```
    $ make server
    $ ./server 9999
    ```

* client
    ```
    $ make client
    $ ./client
    ```
