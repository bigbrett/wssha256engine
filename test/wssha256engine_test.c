#include <openssl/engine.h>
#include <openssl/ossl_typ.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "wssha.h"


static const char* engine_id = "wssha256engine";
static const char correct_digest[32] = {0x6c,0x50,0x76,0x06,0x1b,0x0c,0xc3,0x1f,0x39,0x87,0x76,0x7c,0x06,0x2c,0xd1,0x33,0xab,0x13,0x07,0x34,0xa0,0xb8,0x18,0x4c,0x65,0xd0,0x65,0x88,0x18,0x23,0xb9,0x92};
static const char str[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUV";

int main(int argc, const char* argv[])
{

    printf("Entering engine test program...\n");
    if (argc != 2)
    {
        fprintf(stderr, "*TEST: ERROR! IMPROPER ARGUMENTS TO MAIN\n");
        exit(1);
    }
    // store path to engine shared object
    const char* engine_so_path = argv[1];

    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    // load dynamic engine support
    ENGINE_load_dynamic(); 

    // (copy of the) instance of a generic "dynamic" engine that will magically morph into an instance of our
    // shared library engine once it is loaded by the LOAD command string 
    ENGINE *eng = ENGINE_by_id("dynamic");
    if (eng == NULL)
    {
        fprintf(stderr,"ERROR: Could not load engine \"dynamic\", ENGINE_by_id(\"dynamic\") == NULL\n");
        exit(1);
    }

    // BRIEF: Specify the path to our shared library engine, set the ID, and load it.
    // 
    // The "SO_PATH" control command should be used to identify the
    // shared-library that contains the ENGINE implementation, and "NO_VCHECK"
    // might possibly be useful if there is a minor version conflict and you
    // (or a vendor helpdesk) is convinced you can safely ignore it.
    // "ID" is probably only needed if a shared-library implements
    // multiple ENGINEs, but if you know the engine id you expect to be using,
    // it doesn't hurt to specify it (and this provides a sanity check if
    // nothing else). "LIST_ADD" is only required if you actually wish the
    // loaded ENGINE to be discoverable by application code later on using the
    // ENGINE's "id". For most applications, this isn't necessary - but some
    // application authors may have nifty reasons for using it
    // The "LOAD" command is the only one that takes no parameters and is the command
    // that uses the settings from any previous commands to actually *load*
    // the shared-library ENGINE implementation. If this command succeeds, the
    // (copy of the) 'dynamic' ENGINE will magically morph into the ENGINE
    // that has been loaded from the shared-library. As such, any control
    // commands supported by the loaded ENGINE could then be executed as per
    // normal. Eg. if ENGINE "foo" is implemented in the shared-library
    // "libfoo.so" and it supports some special control command "CMD_FOO", the
    // following code would load and use it (NB: obviously this code has no error checking);
    // 		ENGINE *e = ENGINE_by_id("dynamic");
    // 		ENGINE_ctrl_cmd_string(e, "SO_PATH", "/lib/libfoo.so", 0);
    // 		ENGINE_ctrl_cmd_string(e, "ID", "foo", 0);
    // 		ENGINE_ctrl_cmd_string(e, "LOAD", NULL, 0);
    // 		ENGINE_ctrl_cmd_string(e, "CMD_FOO", "some input data", 0);
    ENGINE_ctrl_cmd_string(eng, "SO_PATH", engine_so_path, 0);
    ENGINE_ctrl_cmd_string(eng, "ID", engine_id, 0);
    ENGINE_ctrl_cmd_string(eng, "LOAD", NULL, 0);
    if (eng == NULL)
    {
        fprintf(stderr,"*TEST: ERROR, COULD NOT LOAD ENGINE:\n\tSO_PATH = %s\n\tID = %s\n", engine_so_path, engine_id);
        exit(1);
    }
    printf("wssha256Engine successfully loaded:\n\tSO_PATH = %s\n\tID = %s\n", engine_so_path, engine_id);

    // initialize engine 
    int status = ENGINE_init(eng); 
    if (status < 0)
    {
        fprintf(stderr,"*TEST: ERROR, COULD NOT INITIALIZE ENGINE\n\tENGINE_init(eng) == %d\n",status);
        exit(1);
    }
    printf("*TEST: Initialized engine [%s]\n\tinit result = %d\n",ENGINE_get_name(eng), status);


    // declare string to hash, and the digest 
    char* str = "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUV";
    unsigned char digest[32]; 
    int str_len = strlen(str);
    unsigned int digest_size = -1; 

    // create sha256 context 
    EVP_MD_CTX *evp_ctx;
    evp_ctx = EVP_MD_CTX_create();

    // GET TIME
    struct timespec start, end;
    uint64_t telapsed;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Compute a message digest indirectly, through the high-level API
    status = EVP_DigestInit_ex(evp_ctx, EVP_sha256(), eng);
    //printf("*TEST: Digest init = %d\n",status);
    status = EVP_DigestUpdate(evp_ctx, (unsigned char*)str, str_len);
    //printf("*TEST: Digest Update = %d\n",status);
    status = EVP_DigestFinal(evp_ctx, digest, &digest_size);
    //printf("*TEST: Digest Final = %d Digest size:%d\n",status,digest_size);
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    telapsed = 1000000000L * (end.tv_sec - start.tv_sec) + end.tv_nsec-start.tv_nsec;
    printf("\n****elapsed time = %llu nanoseconds\n\n", (long long unsigned int) telapsed);
    

    printf("\nRecieved digest = ");
    for(int i= 0; i< digest_size; i++) 
        printf("%x", digest[i]);
    printf("\n");

    // test digest against precomputed result
    int errcnt=0;
    for (int i=0; i < 32; i++)
    {
        if (digest[i] != correct_digest[i])
        {
            errcnt++;
            printf("\t****Error, incorrect digest value at element %i!\n",i);
        }
    }

    // report erroneous values
    if (errcnt == 0)
        printf("****Test status: SUCCESS\n\n");
    else 
    {
        printf("****Test vector status: FAILED\n\n");
        return -1; 
    }  

    EVP_MD_CTX_destroy(evp_ctx); 

    return 0; 
}
