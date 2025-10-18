#include <stdint.h>
#include <string.h>

#include "crypto_core_keccak1600.h"
#include "private/common.h"
#include "shake256_ref.h"

#define SHAKE256_DOMAIN_BYTE_STANDARD 0x1F

int
shake256_ref_init_with_domain(shake256_state_internal *state, unsigned char domain)
{
    crypto_core_keccak1600_init(state->state);
    state->offset = 0;
    state->phase  = SHAKE256_PHASE_ABSORBING;
    state->domain = domain;

    return 0;
}

int
shake256_ref_init(shake256_state_internal *state)
{
    return shake256_ref_init_with_domain(state, SHAKE256_DOMAIN_BYTE_STANDARD);
}

int
shake256_ref_update(shake256_state_internal *state,
                    const unsigned char     *in,
                    unsigned long long       inlen)
{
    size_t i;

    if (state->phase != SHAKE256_PHASE_ABSORBING) {
        return -1; /* Cannot absorb after squeezing */
    }

    for (i = 0; i < inlen; i++) {
        if (state->offset == SHAKE256_RATE) {
            crypto_core_keccak1600_permute_24(state->state);
            state->offset = 0;
        }
        crypto_core_keccak1600_xor_bytes(state->state, &in[i], state->offset, 1);
        state->offset++;
    }

    return 0;
}

static void
shake256_finalize(shake256_state_internal *state)
{
    unsigned char pad;

    /* Apply padding: domain byte at current position, 0x80 at last byte */
    if (state->offset == SHAKE256_RATE - 1) {
        /* Special case: padding fits in one byte */
        pad = state->domain | 0x80;
        crypto_core_keccak1600_xor_bytes(state->state, &pad, state->offset, 1);
    } else {
        /* Normal case: domain and 0x80 at different positions */
        crypto_core_keccak1600_xor_bytes(state->state, &state->domain, state->offset, 1);
        pad = 0x80;
        crypto_core_keccak1600_xor_bytes(state->state, &pad, SHAKE256_RATE - 1, 1);
    }

    /* Final permutation */
    crypto_core_keccak1600_permute_24(state->state);

    state->offset = 0;
    state->phase  = SHAKE256_PHASE_SQUEEZING;
}

int
shake256_ref_final(shake256_state_internal *state, unsigned char *out, size_t outlen)
{
    if (state->phase == SHAKE256_PHASE_ABSORBING) {
        shake256_finalize(state);
    }

    return shake256_ref_squeeze(state, out, outlen);
}

int
shake256_ref_squeeze(shake256_state_internal *state, unsigned char *out, size_t outlen)
{
    size_t i;

    if (state->phase == SHAKE256_PHASE_ABSORBING) {
        shake256_finalize(state);
    }

    for (i = 0; i < outlen; i++) {
        if (state->offset == SHAKE256_RATE) {
            crypto_core_keccak1600_permute_24(state->state);
            state->offset = 0;
        }
        crypto_core_keccak1600_extract_bytes(state->state, &out[i], state->offset, 1);
        state->offset++;
    }

    return 0;
}

int
shake256_ref(unsigned char *out, size_t outlen, const unsigned char *in, unsigned long long inlen)
{
    shake256_state_internal state;

    shake256_ref_init(&state);
    shake256_ref_update(&state, in, inlen);
    shake256_ref_final(&state, out, outlen);

    return 0;
}
