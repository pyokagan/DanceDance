# pyucomm

`ucomm` (`libraries/ucomm`) bindings for Python 3.

## Setup

You first need to build the module (run in this directory):

```shell
python3 setup.py build
```

(If you get an error about `Python.h` being missing, install the `python3-dev` package)

Then, in your code, add this directory to `sys.path` and import the `ucomm` module:
```python
sys.path.insert(0, '/this/directory')
import ucomm
```

## Example

Print all incoming frames:
```python
sys.path.insert(0, '/this/directory')
import ucomm
ucomm.init()  # Always call this function first!
while True:
    frame_data = ucomm.read_frame()  # Returns frame data as bytes
    print(frame_data)
```

Send frames continuously:
```python
sys.path.insert(0, '/this/directory')
import ucomm
ucomm.init()  # Always call this function first!
while True:
    ucomm.write_frame(b'My frame data')
```

## API

### `init()`

```python
ucomm.init() -> None
```
Initializes the library.

**You must call this function first before calling any other function in the library**

### `read_frame()`

```python
ucomm.read_frame() -> bytes
```

Reads a frame from the UART and returns its data.

This function will block until a frame is received successfully.

### `write_frame()`

```python
ucomm.write_frame(data: bytes)
```

Writes a frame to the UART.
