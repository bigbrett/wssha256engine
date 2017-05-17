[![experimental](http://badges.github.io/stability-badges/dist/experimental.svg)](http://github.com/badges/stability-badges)
# wssha256engine
A minimal openSSL engine for offloading sha256 functions to a hardware accelerator in FPGA logic

## Prerequisites
1. You are running linux on the Xilinx ZYNQ-7000 development board, with the necessary design instantiated in PL [link to final design goes here]
2. Ensure you have openSSL using the command `$ openssl version`. If you have lower than version 1.0.2, you must upgrade to this version.
3. Check out the repository using git `$ git clone https://github.com/bigbrett/wssha256engine.git` 

## Building the engine

    $ cd wssha256engine
    $ make

You can verify that the engine can be loaded using: 

    $ openssl engine -t -c `pwd`/bin/libwssha256engine.so
    (/home/brett/wssha256engine/bin/libwssha256engine.so) A test engine for the ws sha256 hardware encryption module, on the Xilinx ZYNQ7000
    Loaded: (wssha256engine) A test engine for the ws sha256 hardware encryption module, on the Xilinx ZYNQ7000
        [ available ]

**Note** You may get an error complaining that `ERROR: Digest is empty! (NID = 0)`. I'm investigating the cause of this, however the engine still loads properly and it does not seem to affect the functionality of the test program 

## Testing the engine
A **quick and easy** test goes like this, where the resultant digest values should be identical: 

(**NOTE THE SIMPLE TEST DOES NOT YET WORK, FOR SOME REASON. RUN THE ADVANCED TEST INSTEAD**)

    $ echo "Hello, Alice and Bob!" | openssl dgst -engine `pwd`/bin/libwssha256engine.so -sha256
      (stdin)= 67bafe3f31a01641bb043233a57f7d90f68052db3d11ef96f76174cb660d5102
    
    $ echo "Hello, Alice and Bob!" | openssl dgst -sha256
      (stdin)= 67bafe3f31a01641bb043233a57f7d90f68052db3d11ef96f76174cb660d5102

A **more advanced** test, using a c test program, can be conducted like this (see test/wssha256engine_test.c for implementation): 
    
    $ make test
    $ source test/runtest.sh

NOTE: the runtest.sh script must remain in the test directory, but should be able to be called from anywhere
    
