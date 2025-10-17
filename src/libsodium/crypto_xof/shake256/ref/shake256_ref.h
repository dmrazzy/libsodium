#ifndef shake256_ref_H
#define shake256_ref_H

#include <stddef.h>
#include <stdint.h>

#define SHAKE256_RATE 136

typedef enum { SHAKE256_PHASE_ABSORBING = 0, SHAKE256_PHASE_SQUEEZING = 1 } shake256_phase;

typedef struct shake256_state_internal_ {
    unsigned char state[200];
    size_t        offset;
    uint8_t       phase;
    unsigned char domain; /* Domain separation byte */
} shake256_state_internal;

int shake256_ref(unsigned char *out, size_t outlen, const unsigned char *in,
                 unsigned long long inlen);

int shake256_ref_init(shake256_state_internal *state);

int shake256_ref_init_with_domain(shake256_state_internal *state, unsigned char domain);

int shake256_ref_update(shake256_state_internal *state,
                        const unsigned char     *in,
                        unsigned long long       inlen);

int shake256_ref_final(shake256_state_internal *state, unsigned char *out, size_t outlen);

int shake256_ref_squeeze(shake256_state_internal *state, unsigned char *out, size_t outlen);

#endif /* shake256_ref_H */
