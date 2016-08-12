# Telit4G-EnergiaLibrary
Texas Instruments Energia Library for the Telit EVK4 BoosterPack containing the LE910SV-V2 LTE module.

## Currently the sendATCommand() function does not work. Substituted is a dummy function that always returns true.

The general structure of the library is like this:

```
LTE_Base  
  | * Will initiate Serial communication with Telit module  
  | * Will define an interface to send/receive AT commands  
  |  
  |--> LTE_HTTP
  |     * Will define an HTTP client/server class
  |     * Abstracts AT commands into GET/POST/PUT/DELETE requests
  |     * Buffer for persistent GET data
  |     * (in the future) support for cookies
  |   
  |--> LTE_VOIP
  |     * Will define a phone class
  |     * dial, hang up, etc
  |
  |--> LTE_SMTP
        * Will define a mail client class
        * select mail server
        * send/fetch
```

Users can then use the provided classes for basic functionality, or use the LTE_Base class to implement their own custom classes that use different AT commands.
