# wssha256engine
A minimal openSSL engine for offloading sha256 functions to a hardware accelerator

## Prerequisites
1. Ensure you have openSSL using the command `$ openssl version`. If you have lower than version 1.0.2, you must upgrade to this version.
2. Check out the repository using git `$ git clone https://github.com/bigbrett/wssha256engine.git` 

## Building the engine

    $ cd wssha256engine.git
    $ make

You can verify that the engine can be loaded using: 

    $ openssl engine -t -c `pwd`/bin/wssha256.so
    (/home/brett/wssha256engine/bin/wssha256engine.so) A test engine for the ws sha256 hardware encryption module, on the Xilinx ZYNQ7000
    Loaded: (wssha256engine) A test engine for the ws sha256 hardware encryption module, on the Xilinx ZYNQ7000
        [ available ]

## Testing the engine
A quick and easy test goes like this:

    $ echo "Hello, Alice and Bob!" | openssl dgst -engine `pwd`/bin/wssha256engine.so -sha256

A more advanced test can be conducted like this: 
    
    $ make test
    $ bin/wssha256engine_test
    
