#ifndef shake128_ref_H
#define shake128_ref_H

#include <stddef.h>
#include <stdint.h>

#define SHAKE128_RATE 168

typedef enum { SHAKE128_PHASE_ABSORBING = 0, SHAKE128_PHASE_SQUEEZING = 1 } shake128_phase;

typedef struct shake128_state_internal_ {
    unsigned char state[200];
    size_t        offset;
    uint8_t       phase;
    unsigned char domain; /* Domain separation byte */
} shake128_state_internal;

int shake128_ref(unsigned char *out, size_t outlen, const unsigned char *in,
                 unsigned long long inlen);

int shake128_ref_init(shake128_state_internal *state);

int shake128_ref_init_with_domain(shake128_state_internal *state, unsigned char domain);

int shake128_ref_update(shake128_state_internal *state,
                        const unsigned char     *in,
                        unsigned long long       inlen);

int shake128_ref_final(shake128_state_internal *state, unsigned char *out, size_t outlen);

int shake128_ref_squeeze(shake128_state_internal *state, unsigned char *out, size_t outlen);

#endif /* shake128_ref_H */
