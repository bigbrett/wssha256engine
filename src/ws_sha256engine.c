#include <openssl/engine.h>
#include <stdio.h>

#define FAIL 0
#define SUCCESS 1

static const char *engine_id = "wssha256";
static const char *engine_name = "A test engine for the ws sha256 hardware encryption module, on the Xilinx ZYNQ7000";


/*
 * Engine Initialization 
 */
int wssha256_init(ENGINE *e)
{
  printf("Initializing wssha256 engine...\n"); 
  return SUCCESS;
}



/*
 *  Engine binding function
 */
static int bind(ENGINE *e, const char *id)
{
  int ret = FAIL;

  if (!ENGINE_set_id(e, engine_id))
  {
    fprintf(stderr, "ENGINE_set_id failed\n");
    goto end;
  }
  if (!ENGINE_set_name(e, engine_name))
  {
    printf("ENGINE_set_name failed\n"); 
    goto end;
  }
  //if (!ENGINE_set_init_function(e, wssha256_init))
  //{
  //  printf("ENGINE_set_name failed\n"); 
  //  goto end;
  //}
  ret = SUCCESS; 
end: 
  return ret; 
}

IMPLEMENT_DYNAMIC_BIND_FN(bind)
IMPLEMENT_DYNAMIC_CHECK_FN()
