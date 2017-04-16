#include <openssl/engine.h>
#include <stdio.h>
#include <string.h>

int main(int argc, const char* argv[])
{
  OpenSSL_add_all_algorithms();
  ERR_load_crypto_strings();
  ENGINE_load_dynamic(); 

  //ENGINE *eng = ENGINE_by_id("wssha256engine");
	ENGINE *eng = ENGINE_by_id("dynamic");
	ENGINE_ctrl_cmd_string(eng, "SO_PATH", "/home/brett/Thesis/openssl_ws/wssha256engine/bin/libwssha256engine.so", 0);
	ENGINE_ctrl_cmd_string(eng, "ID", "wssha256engine", 0);
	ENGINE_ctrl_cmd_string(eng, "LOAD", NULL, 0);

  if (eng == NULL)
  {
    fprintf(stderr,"ERROR, COULD NOT LOAD TEST ENGINE\n");
    exit(1);
  }
  printf("Engine Loaded...\n");

  int status = ENGINE_init(eng); 
  printf("Successfuly initialized engine [%s]\n\tinit result = %d\n",ENGINE_get_name(eng), status);
  return 0; 
}
