# Telit4G-EnergiaLibrary
Texas Instruments Energia Library for the Telit EVK4 BoosterPack containing the LE910SV-V2 LTE module.

The general structure of the library is like this:

```
Makefile
README.md
src/
  |-- LTEBase
  |     * Defines serial communication with Telit module
  |     * Interface to send/receive AT commands
  |-- LTEHttp
  |     * Defines an HTTP client/server class
  |     * Abstracts AT commands into GET/POST/PUT/DELETE requests
  |     * Buffer for persistent GET data
  |
  |-- LTEVoip & LTESmtp
  |     * not yet implemented
test/
  |-- googletest/
  |     * Google Test library
  |
  |-- unit_test.cpp
  |     * Test cases for LTEBase (currently writing more)
  |-- MockSerial
  |     * Emulates the Serial.h library in Energia
  |-- MockEnergia
        * Provides basic functions the Energia IDE contains
```

The user can use the LTEBase/LTEHttp classes to communicate with the Telit BoosterPack. The LTEHttp class abstracts the communication for a simple HTTP server/client. The user can also use the LTEBase class to define his own custom functions (using the sendATCommand() to send AT messages and receive responses).

(For me)
I'm testing this library as I'm writing it, so I'm using the Google Test library along with two custom ones that emulate Energia and Serial communcation. You can run the test cases by running the following command:
```
make clean
make all
./unit_test
```
Or run a specific test (listed in unit_test.cpp) with the command:
```
./unit_test --gtest_filter=TESTGROUP.*
./unit_test --gtest_filter=*.TESTCASE
```
