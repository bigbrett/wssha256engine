#include <openssl/engine.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "wssha.h"

// Turn off this annoying warning that we don't care about 
#pragma GCC diagnostic ignored "-Wsizeof-pointer-memaccess"

#define FAIL 0
#define SUCCESS 1
#define DIGEST_SIZE_BYTES 32
#define MESSAGE_SIZE_BYTES 256

static const char *engine_id = "wssha256";
static const char *engine_name = "A test engine for the ws sha256 hardware encryption module, on the Xilinx ZYNQ7000";
static int wssha256_digest_ids[] = {NID_sha256,0};

static int wssha256engine_sha256_init(EVP_MD_CTX *ctx);
static int wssha256engine_sha256_update(EVP_MD_CTX *ctx, const void *data, size_t count);
static int wssha256engine_sha256_final(EVP_MD_CTX *ctx, unsigned char *md);
int wssha256engine_sha256_copy(EVP_MD_CTX *to, const EVP_MD_CTX *from);
static int wssha256engine_sha256_cleanup(EVP_MD_CTX *ctx);

static const uint32_t digest_len = DIGEST_SIZE_BYTES;

// Create our own message digest hash SHA256 declaration matching that of the generic 
// SHA256 message digest structure (struct env_md_st) defined in openssl/include/evp.h
//struct env_md_st {
//    int type;
//    int pkey_type;
//    int md_size;
//    unsigned long flags;
//    int (*init) (EVP_MD_CTX *ctx);
//    int (*update) (EVP_MD_CTX *ctx, const void *data, size_t count);
//    int (*final) (EVP_MD_CTX *ctx, unsigned char *md);
//    int (*copy) (EVP_MD_CTX *to, const EVP_MD_CTX *from);
//    int (*cleanup) (EVP_MD_CTX *ctx);
//    int (*sign) (int type, const unsigned char *m, unsigned int m_length,
//                 unsigned char *sigret, unsigned int *siglen, void *key);
//    int (*verify) (int type, const unsigned char *m, unsigned int m_length,
//                   const unsigned char *sigbuf, unsigned int siglen,
//                   void *key);
//    int required_pkey_type[5];  /* EVP_PKEY_xxx */
//    int block_size;
//    int ctx_size;               /* how big does the ctx->md_data need to be */
//    int (*md_ctrl) (EVP_MD_CTX *ctx, int cmd, int p1, void *p2); /* control function */
//} 
static EVP_MD wssha256engine_sha256_method = 
{
    NID_sha256, // openSSL algorithm numerical ID
    NID_undef,  // pkey type
    DIGEST_SIZE_BYTES,         // message digest size (32 bytes)
    EVP_MD_FLAG_PKEY_METHOD_SIGNATURE, // flags
    wssha256engine_sha256_init,    // digest initialization function pointer
    wssha256engine_sha256_update,  // digest update function pointer
    wssha256engine_sha256_final,   // digest final function pointer
    wssha256engine_sha256_copy,    // copy function pointer
    wssha256engine_sha256_cleanup, // cleanup function pointer 
    NULL, // function pointer to a function to sign data with a private key  
    NULL, // function pointer to a funtion to verify signed with a public key 
    {NID_undef, NID_undef, 0,0,0}, // required pkey type 
    64, // block size: standard SHA256 uses a block size of 512 Bits = 64 bytes
    32, // context size (how big ctx->md_data must be)
    NULL // pointer to md_ctrl control function 
}; 


/*
 * Digest initialization function
 */
static int wssha256engine_sha256_init(EVP_MD_CTX *ctx)
{
  ctx->update = &wssha256engine_sha256_update;
  if (sha256_init() < 0)
  {
    fprintf(stderr,"ERROR: SHA256 algorithm context could not be initialized\n");
    return FAIL;
  }
  printf("SHA256 algorithm context initialized\n");
  return SUCCESS;
}


/*
 * Digest update function 
 */
static int wssha256engine_sha256_update(EVP_MD_CTX *ctx, const void *data, size_t count)
{
  printf("SHA256 update\n");
  unsigned char *digest = (unsigned char*)malloc(sizeof(unsigned char) * DIGEST_SIZE_BYTES);
  // TODO change below to actual implementation
  //memset(digest, 2, DIGEST_SIZE_BYTES); // set digest to all 2's
  //count = 32; 
  int status = sha256((uint8_t*)data, MESSAGE_SIZE_BYTES, (uint8_t*)digest, (uint32_t*)&digest_len);
  if (status < 0)
  {
    fprintf(stderr,"ERROR: SHA256 algorithm failed\n");
    return FAIL;
  }

  ctx->md_data = digest;
  return SUCCESS;
}


/*
 * Digest final update function 
 * Copies ctx->md_data into output buffer (TODO this is redundant)
 */
static int wssha256engine_sha256_final(EVP_MD_CTX *ctx, unsigned char *md)
{
  printf("SHA256 final: sizeof(EVP_MD)= %lu\n", sizeof(EVP_MD));
  memcpy(md, (unsigned char*)ctx->md_data,DIGEST_SIZE_BYTES);
  return SUCCESS;
}


/*
 * SHA256 EVP_MD_CTX copy function
 */
int wssha256engine_sha256_copy(EVP_MD_CTX *to, const EVP_MD_CTX *from)
{
    printf("Copy SHA256\n");
    if (to->md_data && from->md_data) 
        memcpy(to->md_data, from->md_data,sizeof(from->md_data));
    return SUCCESS;
}


/*
 * SHA256 EVP_MD_CTX cleanup function: sets all fields to zero
 */
static int wssha256engine_sha256_cleanup(EVP_MD_CTX *ctx) 
{
    printf("SHA256 cleanup\n");
    if (ctx->md_data)
        memset(ctx->md_data, 0, 32);
    return SUCCESS;
}


/* 
 * Digest selection function: tells openSSL that whenever a SHA256 digest is 
 * reauested to use our engine implementation 
 */
static int wssha256engine_digest_selector(ENGINE *e, const EVP_MD **digest, const int **nids, int nid)
{
	if (!digest)
  {
    *nids = wssha256_digest_ids;
    printf("ERROR: Digest is empty! (NID = %d)\n",nid);
    return FAIL;
  }

  printf("Digest NID=%d requested\n",nid);

  if (nid == NID_sha256)
  {
    *digest = &wssha256engine_sha256_method; // select our implementation 
    return SUCCESS;
  }
  else
  {
    *digest = NULL;
    return FAIL;
  }
}

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
  if (!ENGINE_set_init_function(e, wssha256_init))
  {
    printf("ENGINE_set_init_function failed\n"); 
    goto end;
  }
  if (!ENGINE_set_digests(e, wssha256engine_digest_selector)) 
  {
    printf("ENGINE_set_digests failed\n");
    goto end;
  }
  ret = SUCCESS; 
end: 
  return ret; 
}

IMPLEMENT_DYNAMIC_BIND_FN(bind)
IMPLEMENT_DYNAMIC_CHECK_FN()
