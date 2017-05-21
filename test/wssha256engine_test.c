#include <openssl/engine.h>
#include <stdio.h>
#include <string.h>

//const char* engine_so_path = "/home/brett/Thesis/openssl_ws/wssha256engine/bin/libwssha256engine.so";
//const char* engine_so_path = "./bin/libwssha256engine.so";

int main(int argc, const char* argv[])
{
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
  ENGINE_ctrl_cmd_string(eng, "ID", "wssha256engine", 0);
  ENGINE_ctrl_cmd_string(eng, "LOAD", NULL, 0);
  if (eng == NULL)
  {
    fprintf(stderr,"*TEST: ERROR, COULD NOT LOAD ENGINE DYNAMIC\n");
    exit(1);
  }
  printf("Engine Loaded...\n");

  // initialize engine 
  int status = ENGINE_init(eng); 
  if (status < 0)
  {
    fprintf(stderr,"*TEST: ERROR, COULD NOT INITIALIZE ENGINE\n");
    exit(1);
  }
  printf("*TEST: Successfuly initialized engine [%s] with init result = %d\n",ENGINE_get_name(eng), status);


  // declare string to hash, and the digest 
  char *str = "Hello, World!";
  unsigned char digest[32]; 
  int str_len = strlen(str);
  unsigned int digest_size = -1; 

  // create sha256 context 
  EVP_MD_CTX *evp_ctx;
  evp_ctx = EVP_MD_CTX_create();

  // Compute a message digest indirectly, through the high-level API
  printf("*TEST: Initializing Digest...\n");
  status = EVP_DigestInit_ex(evp_ctx, EVP_sha256(), eng);
  printf("*TEST: Digest INIT %d\n",status);

  printf("*TEST: Updating Digest...\n");
  status = EVP_DigestUpdate(evp_ctx, (unsigned char*)str, str_len);
  printf("*TEST: Digest Update %d\n",status);

  printf("*TEST: Final Digest...");
  status = EVP_DigestFinal(evp_ctx, digest, &digest_size);
  printf("*TEST: Digest Final %d Digest size:%d\n",status,digest_size);

  for(int i= 0; i< digest_size; i++) 
    printf("%x", digest[i]);
  printf("\n");
  EVP_MD_CTX_destroy(evp_ctx); 

  return 0; 
}
