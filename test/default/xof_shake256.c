
#define TEST_NAME "xof_shake256"
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
    /* Test vectors from NIST FIPS 202 and various sources */
    static const unsigned char msg_empty[] = "";
    static const unsigned char msg_abc[]   = { 0x61, 0x62, 0x63 };
    static const unsigned char msg_fox[]   = { 0x54, 0x68, 0x65, 0x20, 0x71, 0x75, 0x69, 0x63, 0x6b,
                                               0x20, 0x62, 0x72, 0x6f, 0x77, 0x6e, 0x20, 0x66, 0x6f,
                                               0x78, 0x20, 0x6a, 0x75, 0x6d, 0x70, 0x73, 0x20, 0x6f,
                                               0x76, 0x65, 0x72, 0x20, 0x74, 0x68, 0x65, 0x20, 0x6c,
                                               0x61, 0x7a, 0x79, 0x20, 0x64, 0x6f, 0x67 };

    /* SHAKE-256 test vectors */
    static const unsigned char out_empty_32[] = { 0x46, 0xb9, 0xdd, 0x2b, 0x0b, 0xa8, 0x8d, 0x13,
                                                  0x23, 0x3b, 0x3f, 0xeb, 0x74, 0x3e, 0xeb, 0x24,
                                                  0x3f, 0xcd, 0x52, 0xea, 0x62, 0xb8, 0x1b, 0x82,
                                                  0xb5, 0x0c, 0x27, 0x64, 0x6e, 0xd5, 0x76, 0x2f };

    static const unsigned char out_empty_64[] = {
        0x46, 0xb9, 0xdd, 0x2b, 0x0b, 0xa8, 0x8d, 0x13, 0x23, 0x3b, 0x3f, 0xeb, 0x74,
        0x3e, 0xeb, 0x24, 0x3f, 0xcd, 0x52, 0xea, 0x62, 0xb8, 0x1b, 0x82, 0xb5, 0x0c,
        0x27, 0x64, 0x6e, 0xd5, 0x76, 0x2f, 0xd7, 0x5d, 0xc4, 0xdd, 0xd8, 0xc0, 0xf2,
        0x00, 0xcb, 0x05, 0x01, 0x9d, 0x67, 0xb5, 0x92, 0xf6, 0xfc, 0x82, 0x1c, 0x49,
        0x47, 0x9a, 0xb4, 0x86, 0x40, 0x29, 0x2e, 0xac, 0xb3, 0xb7, 0xc4, 0xbe
    };

    static const unsigned char out_abc_32[] = { 0x48, 0x33, 0x66, 0x60, 0x13, 0x60, 0xa8, 0x77,
                                                0x1c, 0x68, 0x63, 0x08, 0x0c, 0xc4, 0x11, 0x4d,
                                                0x8d, 0xb4, 0x45, 0x30, 0xf8, 0xf1, 0xe1, 0xee,
                                                0x4f, 0x94, 0xea, 0x37, 0xe7, 0x8b, 0x57, 0x39 };

    static const unsigned char out_fox_32[] = { 0x2f, 0x67, 0x13, 0x43, 0xd9, 0xb2, 0xe1, 0x60,
                                                0x4d, 0xc9, 0xdc, 0xf0, 0x75, 0x3e, 0x5f, 0xe1,
                                                0x5c, 0x7c, 0x64, 0xa0, 0xd2, 0x83, 0xcb, 0xbf,
                                                0x72, 0x2d, 0x41, 0x1a, 0x0e, 0x36, 0xf6, 0xca };

    static const unsigned char out_fox_64[] = {
        0x2f, 0x67, 0x13, 0x43, 0xd9, 0xb2, 0xe1, 0x60, 0x4d, 0xc9, 0xdc, 0xf0, 0x75,
        0x3e, 0x5f, 0xe1, 0x5c, 0x7c, 0x64, 0xa0, 0xd2, 0x83, 0xcb, 0xbf, 0x72, 0x2d,
        0x41, 0x1a, 0x0e, 0x36, 0xf6, 0xca, 0x1d, 0x01, 0xd1, 0x36, 0x9a, 0x23, 0x53,
        0x9c, 0xd8, 0x0f, 0x7c, 0x05, 0x4b, 0x6e, 0x5d, 0xaf, 0x9c, 0x96, 0x2c, 0xad,
        0x5b, 0x8e, 0xd5, 0xbd, 0x11, 0x99, 0x8b, 0x40, 0xd5, 0x73, 0x44, 0x42
    };

    testvector vectors[] = { { msg_empty, 0, out_empty_32, 32 },
                             { msg_empty, 0, out_empty_64, 64 },
                             { msg_abc, 3, out_abc_32, 32 },
                             { msg_fox, 43, out_fox_32, 32 },
                             { msg_fox, 43, out_fox_64, 64 } };

    unsigned char             out[256];
    crypto_xof_shake256_state state;
    size_t                    i;

    /* Test constants */
    assert(crypto_xof_shake256_blockbytes() == 136);
    assert(crypto_xof_shake256_statebytes() == 256);
    assert(crypto_xof_shake256_domain_standard() == 0x1F);

    /* Test one-shot API */
    for (i = 0; i < sizeof(vectors) / sizeof(vectors[0]); i++) {
        crypto_xof_shake256(out, vectors[i].out_len, vectors[i].msg, vectors[i].msg_len);
        if (memcmp(out, vectors[i].out, vectors[i].out_len) != 0) {
            printf("Test vector %zu failed (one-shot)\n", i);
            return 1;
        }
    }

    /* Test streaming API */
    for (i = 0; i < sizeof(vectors) / sizeof(vectors[0]); i++) {
        crypto_xof_shake256_init(&state);
        crypto_xof_shake256_update(&state, vectors[i].msg, vectors[i].msg_len);
        crypto_xof_shake256_final(&state, out, vectors[i].out_len);
        if (memcmp(out, vectors[i].out, vectors[i].out_len) != 0) {
            printf("Test vector %zu failed (streaming)\n", i);
            return 1;
        }
    }

    /* Test multiple squeeze calls */
    crypto_xof_shake256_init(&state);
    crypto_xof_shake256_update(&state, msg_abc, 3);
    crypto_xof_shake256_final(&state, out, 16);
    crypto_xof_shake256_squeeze(&state, out + 16, 16);
    if (memcmp(out, out_abc_32, 32) != 0) {
        printf("Multiple squeeze test failed\n");
        return 1;
    }

    /* Test custom domain byte produces different output */
    crypto_xof_shake256_init(&state);
    crypto_xof_shake256_update(&state, msg_abc, 3);
    crypto_xof_shake256_final(&state, out, 32);

    crypto_xof_shake256_init_with_domain(&state, 0x99);
    crypto_xof_shake256_update(&state, msg_abc, 3);
    crypto_xof_shake256_final(&state, out + 32, 32);

    if (memcmp(out, out + 32, 32) == 0) {
        printf("Custom domain byte test failed (outputs should differ)\n");
        return 1;
    }

    /* Test standard domain constant */
    crypto_xof_shake256_init_with_domain(&state, crypto_xof_shake256_domain_standard());
    crypto_xof_shake256_update(&state, msg_abc, 3);
    crypto_xof_shake256_final(&state, out + 64, 32);

    if (memcmp(out, out + 64, 32) != 0) {
        printf("Domain constant test failed (should match standard init)\n");
        return 1;
    }

    /* Test cannot absorb after squeezing */
    crypto_xof_shake256_init(&state);
    crypto_xof_shake256_final(&state, out, 32);
    if (crypto_xof_shake256_update(&state, msg_abc, 3) == 0) {
        printf("Should not be able to absorb after squeezing\n");
        return 1;
    }

    printf("All SHAKE-256 tests passed\n");
    return 0;
}
