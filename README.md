# Telit4G-EnergiaLibrary
Texas Instruments Energia Library for the Telit EVK4 BoosterPack containing the LE910SV-V2 LTE module.

The general structure of the library is like this:

```
src/
  |-- LTE_Base
  |     * Defines serial communication with Telit module
  |     * Interface to send/receive AT commands
  |-- LTE_TCP
  |     * Defines a TCP connection class
  |     * Connect to a socket and send/receive information
  |     * Buffer for persistent receive data
  |
examples/
```

The user can use the LTE_Base/LTE_TCP classes to communicate with the Telit BoosterPack. The Telit modem understands AT Commands (details can be found in the Telit AT Command Reference Guide: http://www.telit.com/fileadmin/user_upload/products/Downloads/4G/Telit_LE910_V2_Series_AT_Commands_Reference_Guide_r2.pdf). The LTE_TCP class lets users send/receive data over a TCP socket. The user can also use the LTE_Base class to define his own custom functions (using the sendATCommand() to send AT messages and receive responses from the modem).

Check out the examples/ folder to get started with this library. There you can find use cases for both LTE_Base and LTE_TCP classes. For more details about specific funtions, docstrings are included in the source code.

The Telit EVK4 comes with several on-board sensors. The libraries for these are provided by Telit Communications PLC, and are not provided/needed by this library (except for the IoTBluemix example).
