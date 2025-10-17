
#define TEST_NAME "xof_shake128"
#include "cmptest.h"

typedef struct {
    const unsigned char *msg;
    size_t               msg_len;
    const unsigned char *out;
    size_t               out_len;
} testvector;

int
main(void)
{
    /* Test vectors from NIST and various sources */
    static const unsigned char msg_empty[] = "";
    static const unsigned char msg_abc[]   = { 0x61, 0x62, 0x63 };
    static const unsigned char msg_fox[]   = { 0x54, 0x68, 0x65, 0x20, 0x71, 0x75, 0x69, 0x63, 0x6b,
                                               0x20, 0x62, 0x72, 0x6f, 0x77, 0x6e, 0x20, 0x66, 0x6f,
                                               0x78, 0x20, 0x6a, 0x75, 0x6d, 0x70, 0x73, 0x20, 0x6f,
                                               0x76, 0x65, 0x72, 0x20, 0x74, 0x68, 0x65, 0x20, 0x6c,
                                               0x61, 0x7a, 0x79, 0x20, 0x64, 0x6f, 0x67 };

    static const unsigned char out_empty_32[] = { 0x7f, 0x9c, 0x2b, 0xa4, 0xe8, 0x8f, 0x82, 0x7d,
                                                  0x61, 0x60, 0x45, 0x50, 0x76, 0x05, 0x85, 0x3e,
                                                  0xd7, 0x3b, 0x80, 0x93, 0xf6, 0xef, 0xbc, 0x88,
                                                  0xeb, 0x1a, 0x6e, 0xac, 0xfa, 0x66, 0xef, 0x26 };

    static const unsigned char out_empty_64[] = {
        0x7f, 0x9c, 0x2b, 0xa4, 0xe8, 0x8f, 0x82, 0x7d, 0x61, 0x60, 0x45, 0x50, 0x76,
        0x05, 0x85, 0x3e, 0xd7, 0x3b, 0x80, 0x93, 0xf6, 0xef, 0xbc, 0x88, 0xeb, 0x1a,
        0x6e, 0xac, 0xfa, 0x66, 0xef, 0x26, 0x3c, 0xb1, 0xee, 0xa9, 0x88, 0x00, 0x4b,
        0x93, 0x10, 0x3c, 0xfb, 0x0a, 0xee, 0xfd, 0x2a, 0x68, 0x6e, 0x01, 0xfa, 0x4a,
        0x58, 0xe8, 0xa3, 0x63, 0x9c, 0xa8, 0xa1, 0xe3, 0xf9, 0xae, 0x57, 0xe2
    };

    static const unsigned char out_abc_32[] = { 0x58, 0x81, 0x09, 0x2d, 0xd8, 0x18, 0xbf, 0x5c,
                                                0xf8, 0xa3, 0xdd, 0xb7, 0x93, 0xfb, 0xcb, 0xa7,
                                                0x40, 0x97, 0xd5, 0xc5, 0x26, 0xa6, 0xd3, 0x5f,
                                                0x97, 0xb8, 0x33, 0x51, 0x94, 0x0f, 0x2c, 0xc8 };

    static const unsigned char out_fox_32[] = { 0xf4, 0x20, 0x2e, 0x3c, 0x58, 0x52, 0xf9, 0x18,
                                                0x2a, 0x04, 0x30, 0xfd, 0x81, 0x44, 0xf0, 0xa7,
                                                0x4b, 0x95, 0xe7, 0x41, 0x7e, 0xca, 0xe1, 0x7d,
                                                0xb0, 0xf8, 0xcf, 0xee, 0xd0, 0xe3, 0xe6, 0x6e };

    static const unsigned char out_fox_64[] = {
        0xf4, 0x20, 0x2e, 0x3c, 0x58, 0x52, 0xf9, 0x18, 0x2a, 0x04, 0x30, 0xfd, 0x81,
        0x44, 0xf0, 0xa7, 0x4b, 0x95, 0xe7, 0x41, 0x7e, 0xca, 0xe1, 0x7d, 0xb0, 0xf8,
        0xcf, 0xee, 0xd0, 0xe3, 0xe6, 0x6e, 0xb5, 0x58, 0x5e, 0xc6, 0xf8, 0x60, 0x21,
        0xca, 0xcf, 0x27, 0x2c, 0x79, 0x8b, 0xcf, 0x97, 0xd3, 0x68, 0xb8, 0x86, 0xb1,
        0x8f, 0xec, 0x3a, 0x57, 0x1f, 0x09, 0x60, 0x86, 0xa5, 0x23, 0x71, 0x7a
    };

    testvector vectors[] = { { msg_empty, 0, out_empty_32, 32 },
                             { msg_empty, 0, out_empty_64, 64 },
                             { msg_abc, 3, out_abc_32, 32 },
                             { msg_fox, 43, out_fox_32, 32 },
                             { msg_fox, 43, out_fox_64, 64 } };

    unsigned char             out[256];
    crypto_xof_shake128_state state;
    size_t                    i, j;

    /* Test constants */
    assert(crypto_xof_shake128_blockbytes() == 168);
    assert(crypto_xof_shake128_statebytes() == 256);
    assert(crypto_xof_shake128_domain_standard() == 0x1F);

    /* Test one-shot API */
    for (i = 0; i < sizeof(vectors) / sizeof(vectors[0]); i++) {
        crypto_xof_shake128(out, vectors[i].out_len, vectors[i].msg, vectors[i].msg_len);
        if (memcmp(out, vectors[i].out, vectors[i].out_len) != 0) {
            printf("Test vector %zu failed (one-shot)\n", i);
            return 1;
        }
    }

    /* Test streaming API */
    for (i = 0; i < sizeof(vectors) / sizeof(vectors[0]); i++) {
        crypto_xof_shake128_init(&state);
        crypto_xof_shake128_update(&state, vectors[i].msg, vectors[i].msg_len);
        crypto_xof_shake128_final(&state, out, vectors[i].out_len);
        if (memcmp(out, vectors[i].out, vectors[i].out_len) != 0) {
            printf("Test vector %zu failed (streaming)\n", i);
            return 1;
        }
    }

    /* Test multiple squeeze calls */
    crypto_xof_shake128_init(&state);
    crypto_xof_shake128_update(&state, msg_abc, 3);
    crypto_xof_shake128_final(&state, out, 16);
    crypto_xof_shake128_squeeze(&state, out + 16, 16);
    if (memcmp(out, out_abc_32, 32) != 0) {
        printf("Multiple squeeze test failed\n");
        return 1;
    }

    /* Test custom domain byte produces different output */
    crypto_xof_shake128_init(&state);
    crypto_xof_shake128_update(&state, msg_abc, 3);
    crypto_xof_shake128_final(&state, out, 32);

    crypto_xof_shake128_init_with_domain(&state, 0x99);
    crypto_xof_shake128_update(&state, msg_abc, 3);
    crypto_xof_shake128_final(&state, out + 32, 32);

    if (memcmp(out, out + 32, 32) == 0) {
        printf("Custom domain byte test failed (outputs should differ)\n");
        return 1;
    }

    /* Test standard domain constant */
    crypto_xof_shake128_init_with_domain(&state, crypto_xof_shake128_domain_standard());
    crypto_xof_shake128_update(&state, msg_abc, 3);
    crypto_xof_shake128_final(&state, out + 64, 32);

    if (memcmp(out, out + 64, 32) != 0) {
        printf("Domain constant test failed (should match standard init)\n");
        return 1;
    }

    /* Test cannot absorb after squeezing */
    crypto_xof_shake128_init(&state);
    crypto_xof_shake128_final(&state, out, 32);
    if (crypto_xof_shake128_update(&state, msg_abc, 3) == 0) {
        printf("Should not be able to absorb after squeezing\n");
        return 1;
    }

    printf("All SHAKE-128 tests passed\n");
    return 0;
}
