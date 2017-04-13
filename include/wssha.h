#pragma once

/* SHA-256 -- returns a status value */
int32_t sha256_init(void);
int32_t sha256(uint8_t *datap, uint64_t datalen,uint8_t *digestp, uint32_t *digest_lenp);

