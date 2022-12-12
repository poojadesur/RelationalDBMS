# Report Part 2
### By-
- Ritvik Aryan Kalra, 2019115002
- Pooja Desur, 2019101112

## Implementation of the code
To implement this, we used a POSIX system call namely `flock`. `flock` allows us to apply or remove and advisory lock on an opened file. So how our mechanism goes is, that whenever a **read** or a **write** operation is being performed on a page of table, we lock the page on which the read or write operation is being performed. As the file is locked any other process that needs to access the file will have to wait to acquire the lock and then only can continue with the process.

## Results
The following is one of the results. In this we run the `UPDATE` command on the left instance of the server, and while the update is running we run the `PRINT` command on the right instance. While the update is being made, the right instance of the terminal would be on hold and only when the update is complete will the right instance of the command be able to read the page that needs to be accessed.
![results](./DS%20Phase%203%20Part%202%20Results.png)

[Video Link](https://youtu.be/cc54_TkSSIA)