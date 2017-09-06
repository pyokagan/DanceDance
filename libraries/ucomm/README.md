# ucomm

Arduino <-> Raspberry PI communications library.

Compiles on both Arduino and Raspberry Pi.

## Examples

The `src` directory will need to be in your include path.
Alternatively, you can use a relative path i.e. `#include "../../libraries/ucomm/src/ucomm.h"`

Read a frame:
```c
#include <ucomm.h>

int main(int argc, char *argv[]) {
    ucomm_init(); // Always call this function first!

    // Buffer with 100 bytes, which will receive the frame data.
    // The buffer size must be at least 2 bytes and cannot exceed 255 bytes.
    uint8_t nbytes;

    // uframe_read() will block until it receives an uncorrupted frame.
    // It returns the number of bytes inside the frame.
    nbytes = uframe_read(buf, sizeof(buf));

    // print the bytes read to stdout
    fwrite(buf, 1, nbytes, stdout);

    return 0;
}
```

Write a frame:
```c
#include <ucomm.h>

int main(int argc, char *argv[]) {
    ucomm_init(); // Always call this function first!

    // Buffer initialized with the data you want to send.
    // The buffer size cannot exceed 255 bytes.
    char buf[] = "Some message";

    // Write the data
    uframe_write(buf, sizeof(buf));

    return 0;
}
```
