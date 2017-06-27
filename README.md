[![experimental](http://badges.github.io/stability-badges/dist/experimental.svg)](http://github.com/badges/stability-badges)
# wssha256engine
A minimal openSSL engine for offloading sha256 functions to a hardware accelerator in FPGA logic

## Prerequisites
1. You are running linux on the Xilinx ZYNQ-7000 development board, with the necessary design instantiated in PL [link to final design goes here]
2. Ensure you have openSSL using the command `$ openssl version`. If you have lower than version 1.0.2, you must upgrade to this version.
3. Check out the repository using git `$ git clone https://github.com/bigbrett/wssha256engine.git` 
4. You must have the wssha256kern kernel module loaded (source to come...)

## Building the engine

    $ cd wssha256engine
    $ make

You can verify that the engine can be loaded using: 

    $ openssl engine -t -c `pwd`/bin/libwssha256engine.so
    (/home/brett/wssha256engine/bin/libwssha256engine.so) A test engine for the ws sha256 hardware encryption module, on the Xilinx ZYNQ7000
    Loaded: (wssha256engine) A test engine for the ws sha256 hardware encryption module, on the Xilinx ZYNQ7000
        [ available ]

## Testing the engine
### Quck test
A quick and easy test goes like this, where the resultant digest values should be identical: 

    $ teststr="OpenSSL has poor documentation!"
    
    $ echo -n $teststr | openssl dgst -engine /full/path/to/libwssha256engine.so -sha256
      (stdin)= b6264baa6804978da052f91344621c81fe7653847012481ddb5567376dec2839
    
    $ echo -n $teststr | openssl dgst -sha256
      (stdin)= b6264baa6804978da052f91344621c81fe7653847012481ddb5567376dec2839

### Custom Test
A more advanced test, using a c test program, can be conducted like this (see test/wssha256engine_test.c for implementation): 
    
    $ make test
    $ source test/runtest.sh

NOTE: the runtest.sh script must remain in the test directory, but should be able to be called from anywhere
    
### OpenSSL speed test
The speed of the engine's digest computation can be tested using the built-in openSSL speed command (making sure to explicitly specify using the EVP API for the message digest)

    $ openssl speed -evp sha256 -engine /path/to/libwssha256engine.so


