# MapReduce engine

This is a MapReduce engine running on the same Linux machine (as a separate process) as the clients requesting the tasks.
The client has to create a shared library, defining four functions (with the appropriate types): `map`, `reduce`, `read`, `write`.

* `map` and `reduce` are the usual mandatory functions, required by the MapReduce programming model

* `read` is used by the server to read the input data

* `write` is used by the server to write the output data

The client then sends a request to the server and the server executes the client's task.

### How to build:

Clone the repo. In the repo, execute:
```
mkdir build
cd build
cmake ..
make
```

### How to run:

In a terminal, navigate to the build directory. Execute:

`./bin/server`

The server creates a local socket file named **server_socket** in the build directory.
If for any reason the process terminates abnormally (e.g.: a signal, an unhandled exception), the
local socket file won't be unlinked and it needs to be removed manually (`rm socket_file`).

While the server is running, several commands can be processed by the command line interface:

* `quit`: waits for the current task (if any) to finish and exits the application

* `task info`: prints information about the current task (if any) to the standart output

* `task stop`: interrupts the current task (if any) prematurely

In a separate terminal, navigate to the build directory. Execute:

`./bin/client_example <task> <threads>`

where `<task>` is the task to be executed by the server and `<threads>` is the number of threads to be used for the executing.

Currently, the task examples in the repo are:

* **word_count**

If you want to add your own tasks, you will have to modify the CMakeLists.txt in the **client_example** source directory and write your code for the shared library.
