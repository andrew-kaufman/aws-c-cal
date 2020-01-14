/*
 * Copyright 2010-2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
#include <aws/cal/ecc.h>
#include <aws/cal/hash.h>
#include <aws/common/byte_buf.h>
#include <aws/testing/aws_test_harness.h>

static int s_test_key_derivation(
    struct aws_allocator *allocator,
    enum aws_ecc_curve_name curve_name,
    struct aws_byte_cursor private_key,
    struct aws_byte_cursor expected_pub_x,
    struct aws_byte_cursor expected_pub_y) {

    struct aws_ecc_key_pair *private_key_pair =
        aws_ecc_key_pair_new_from_private_key(allocator, curve_name, &private_key);

    ASSERT_NOT_NULL(private_key_pair);

    int error = aws_ecc_key_pair_derive_public_key(private_key_pair);

    /* this isn't supported on Apple platforms, since AFAIK it isn't possible */
    if (error) {
        ASSERT_INT_EQUALS(AWS_ERROR_UNSUPPORTED_OPERATION, aws_last_error());
        goto complete;
    }

    struct aws_byte_cursor pub_x;
    struct aws_byte_cursor pub_y;

    aws_ecc_key_pair_get_public_key(private_key_pair, &pub_x, &pub_y);
    ASSERT_BIN_ARRAYS_EQUALS(expected_pub_x.ptr, expected_pub_x.len, pub_x.ptr, pub_x.len);

    ASSERT_BIN_ARRAYS_EQUALS(expected_pub_y.ptr, expected_pub_y.len, pub_y.ptr, pub_y.len);

complete:
    aws_ecc_key_pair_destroy(private_key_pair);

    return AWS_OP_SUCCESS;
}

static int s_ecdsa_p256_test_pub_key_derivation_fn(struct aws_allocator *allocator, void *ctx) {
    (void)ctx;

    uint8_t d[] = {
        0xc9, 0x80, 0x68, 0x98, 0xa0, 0x33, 0x49, 0x16, 0xc8, 0x60, 0x74, 0x88, 0x80, 0xa5, 0x41, 0xf0,
        0x93, 0xb5, 0x79, 0xa9, 0xb1, 0xf3, 0x29, 0x34, 0xd8, 0x6c, 0x36, 0x3c, 0x39, 0x80, 0x03, 0x57,
    };

    struct aws_byte_cursor private_key = aws_byte_cursor_from_array(d, sizeof(d));

    uint8_t x[] = {
        0xd0, 0x72, 0x0d, 0xc6, 0x91, 0xaa, 0x80, 0x09, 0x6b, 0xa3, 0x2f, 0xed, 0x1c, 0xb9, 0x7c, 0x2b,
        0x62, 0x06, 0x90, 0xd0, 0x6d, 0xe0, 0x31, 0x7b, 0x86, 0x18, 0xd5, 0xce, 0x65, 0xeb, 0x72, 0x8f,
    };

    struct aws_byte_cursor pub_x = aws_byte_cursor_from_array(x, sizeof(x));

    uint8_t y[] = {
        0x96, 0x81, 0xb5, 0x17, 0xb1, 0xcd, 0xa1, 0x7d, 0x0d, 0x83, 0xd3, 0x35, 0xd9, 0xc4, 0xa8, 0xa9,
        0xa9, 0xb0, 0xb1, 0xb3, 0xc7, 0x10, 0x6d, 0x8f, 0x3c, 0x72, 0xbc, 0x50, 0x93, 0xdc, 0x27, 0x5f,
    };

    struct aws_byte_cursor pub_y = aws_byte_cursor_from_array(y, sizeof(y));

    return s_test_key_derivation(allocator, AWS_CAL_ECDSA_P256, private_key, pub_x, pub_y);
}

AWS_TEST_CASE(ecdsa_p256_test_pub_key_derivation, s_ecdsa_p256_test_pub_key_derivation_fn)

static int s_ecdsa_p384_test_pub_key_derivation_fn(struct aws_allocator *allocator, void *ctx) {
    (void)ctx;

    uint8_t d[] = {
        0x53, 0x94, 0xf7, 0x97, 0x3e, 0xa8, 0x68, 0xc5, 0x2b, 0xf3, 0xff, 0x8d, 0x8c, 0xee, 0xb4, 0xdb,
        0x90, 0xa6, 0x83, 0x65, 0x3b, 0x12, 0x48, 0x5d, 0x5f, 0x62, 0x7c, 0x3c, 0xe5, 0xab, 0xd8, 0x97,
        0x8f, 0xc9, 0x67, 0x3d, 0x14, 0xa7, 0x1d, 0x92, 0x57, 0x47, 0x93, 0x16, 0x62, 0x49, 0x3c, 0x37,
    };

    struct aws_byte_cursor private_key = aws_byte_cursor_from_array(d, sizeof(d));

    uint8_t x[] = {
        0xfd, 0x3c, 0x84, 0xe5, 0x68, 0x9b, 0xed, 0x27, 0x0e, 0x60, 0x1b, 0x3d, 0x80, 0xf9, 0x0d, 0x67,
        0xa9, 0xae, 0x45, 0x1c, 0xce, 0x89, 0x0f, 0x53, 0xe5, 0x83, 0x22, 0x9a, 0xd0, 0xe2, 0xee, 0x64,
        0x56, 0x11, 0xfa, 0x99, 0x36, 0xdf, 0xa4, 0x53, 0x06, 0xec, 0x18, 0x06, 0x67, 0x74, 0xaa, 0x24,
    };

    struct aws_byte_cursor pub_x = aws_byte_cursor_from_array(x, sizeof(x));

    uint8_t y[] = {
        0xb8, 0x3c, 0xa4, 0x12, 0x6c, 0xfc, 0x4c, 0x4d, 0x1d, 0x18, 0xa4, 0xb6, 0xc2, 0x1c, 0x7f, 0x69,
        0x9d, 0x51, 0x23, 0xdd, 0x9c, 0x24, 0xf6, 0x6f, 0x83, 0x38, 0x46, 0xee, 0xb5, 0x82, 0x96, 0x19,
        0x6b, 0x42, 0xec, 0x06, 0x42, 0x5d, 0xb5, 0xb7, 0x0a, 0x4b, 0x81, 0xb7, 0xfc, 0xf7, 0x05, 0xa0,
    };

    struct aws_byte_cursor pub_y = aws_byte_cursor_from_array(y, sizeof(y));

    return s_test_key_derivation(allocator, AWS_CAL_ECDSA_P384, private_key, pub_x, pub_y);
}

AWS_TEST_CASE(ecdsa_p384_test_pub_key_derivation, s_ecdsa_p384_test_pub_key_derivation_fn)

static int s_test_known_signing_value(
    struct aws_allocator *allocator,
    enum aws_ecc_curve_name curve_name,
    struct aws_byte_cursor private_key,
    struct aws_byte_cursor pub_x,
    struct aws_byte_cursor pub_y) {

    struct aws_ecc_key_pair *signing_key = aws_ecc_key_pair_new_from_private_key(allocator, curve_name, &private_key);
    ASSERT_NOT_NULL(signing_key);

    struct aws_ecc_key_pair *verifying_key =
        aws_ecc_key_pair_new_from_public_key(allocator, curve_name, &pub_x, &pub_y);
    ASSERT_NOT_NULL(verifying_key);

    uint8_t message[] = {
        0x59, 0x05, 0x23, 0x88, 0x77, 0xc7, 0x74, 0x21, 0xf7, 0x3e, 0x43, 0xee, 0x3d, 0xa6, 0xf2, 0xd9,
        0xe2, 0xcc, 0xad, 0x5f, 0xc9, 0x42, 0xdc, 0xec, 0x0c, 0xbd, 0x25, 0x48, 0x29, 0x35, 0xfa, 0xaf,
        0x41, 0x69, 0x83, 0xfe, 0x16, 0x5b, 0x1a, 0x04, 0x5e, 0xe2, 0xbc, 0xd2, 0xe6, 0xdc, 0xa3, 0xbd,
        0xf4, 0x6c, 0x43, 0x10, 0xa7, 0x46, 0x1f, 0x9a, 0x37, 0x96, 0x0c, 0xa6, 0x72, 0xd3, 0xfe, 0xb5,
        0x47, 0x3e, 0x25, 0x36, 0x05, 0xfb, 0x1d, 0xdf, 0xd2, 0x80, 0x65, 0xb5, 0x3c, 0xb5, 0x85, 0x8a,
        0x8a, 0xd2, 0x81, 0x75, 0xbf, 0x9b, 0xd3, 0x86, 0xa5, 0xe4, 0x71, 0xea, 0x7a, 0x65, 0xc1, 0x7c,
        0xc9, 0x34, 0xa9, 0xd7, 0x91, 0xe9, 0x14, 0x91, 0xeb, 0x37, 0x54, 0xd0, 0x37, 0x99, 0x79, 0x0f,
        0xe2, 0xd3, 0x08, 0xd1, 0x61, 0x46, 0xd5, 0xc9, 0xb0, 0xd0, 0xde, 0xbd, 0x97, 0xd7, 0x9c, 0xe8,
    };
    struct aws_byte_cursor message_input = aws_byte_cursor_from_array(message, sizeof(message));
    uint8_t hash[AWS_SHA256_LEN];
    AWS_ZERO_ARRAY(hash);
    struct aws_byte_buf hash_value = aws_byte_buf_from_empty_array(hash, sizeof(hash));
    aws_sha256_compute(allocator, &message_input, &hash_value, 0);

    size_t signature_size = aws_ecc_key_pair_signature_length(signing_key);

    struct aws_byte_buf signature_buf;
    AWS_ZERO_STRUCT(signature_buf);
    aws_byte_buf_init(&signature_buf, allocator, signature_size);

    struct aws_byte_cursor hash_cur = aws_byte_cursor_from_buf(&hash_value);
    ASSERT_SUCCESS(aws_ecc_key_pair_sign_message(signing_key, &hash_cur, &signature_buf));

    struct aws_byte_cursor signature_cur = aws_byte_cursor_from_buf(&signature_buf);
    ASSERT_SUCCESS(aws_ecc_key_pair_verify_signature(verifying_key, &hash_cur, &signature_cur));

    aws_byte_buf_clean_up(&signature_buf);
    aws_ecc_key_pair_destroy(verifying_key);
    aws_ecc_key_pair_destroy(signing_key);

    return AWS_OP_SUCCESS;
}

static int s_ecdsa_p256_test_known_signing_value_fn(struct aws_allocator *allocator, void *ctx) {
    (void)ctx;

    uint8_t d[] = {
        0x51, 0x9b, 0x42, 0x3d, 0x71, 0x5f, 0x8b, 0x58, 0x1f, 0x4f, 0xa8, 0xee, 0x59, 0xf4, 0x77, 0x1a,
        0x5b, 0x44, 0xc8, 0x13, 0x0b, 0x4e, 0x3e, 0xac, 0xca, 0x54, 0xa5, 0x6d, 0xda, 0x72, 0xb4, 0x64,
    };

    struct aws_byte_cursor private_key = aws_byte_cursor_from_array(d, sizeof(d));

    uint8_t x[] = {
        0x1c, 0xcb, 0xe9, 0x1c, 0x07, 0x5f, 0xc7, 0xf4, 0xf0, 0x33, 0xbf, 0xa2, 0x48, 0xdb, 0x8f, 0xcc,
        0xd3, 0x56, 0x5d, 0xe9, 0x4b, 0xbf, 0xb1, 0x2f, 0x3c, 0x59, 0xff, 0x46, 0xc2, 0x71, 0xbf, 0x83,
    };

    uint8_t y[] = {
        0xce, 0x40, 0x14, 0xc6, 0x88, 0x11, 0xf9, 0xa2, 0x1a, 0x1f, 0xdb, 0x2c, 0x0e, 0x61, 0x13, 0xe0,
        0x6d, 0xb7, 0xca, 0x93, 0xb7, 0x40, 0x4e, 0x78, 0xdc, 0x7c, 0xcd, 0x5c, 0xa8, 0x9a, 0x4c, 0xa9,
    };

    struct aws_byte_cursor pub_x = aws_byte_cursor_from_array(x, sizeof(x));
    struct aws_byte_cursor pub_y = aws_byte_cursor_from_array(y, sizeof(y));

    return s_test_known_signing_value(allocator, AWS_CAL_ECDSA_P256, private_key, pub_x, pub_y);
}

AWS_TEST_CASE(ecdsa_p256_test_known_signing_value, s_ecdsa_p256_test_known_signing_value_fn)

static int s_ecdsa_p384_test_known_signing_value_fn(struct aws_allocator *allocator, void *ctx) {
    (void)ctx;

    uint8_t d[] = {
        0x53, 0x94, 0xf7, 0x97, 0x3e, 0xa8, 0x68, 0xc5, 0x2b, 0xf3, 0xff, 0x8d, 0x8c, 0xee, 0xb4, 0xdb,
        0x90, 0xa6, 0x83, 0x65, 0x3b, 0x12, 0x48, 0x5d, 0x5f, 0x62, 0x7c, 0x3c, 0xe5, 0xab, 0xd8, 0x97,
        0x8f, 0xc9, 0x67, 0x3d, 0x14, 0xa7, 0x1d, 0x92, 0x57, 0x47, 0x93, 0x16, 0x62, 0x49, 0x3c, 0x37,
    };

    struct aws_byte_cursor private_key = aws_byte_cursor_from_array(d, sizeof(d));

    uint8_t x[] = {
        0xfd, 0x3c, 0x84, 0xe5, 0x68, 0x9b, 0xed, 0x27, 0x0e, 0x60, 0x1b, 0x3d, 0x80, 0xf9, 0x0d, 0x67,
        0xa9, 0xae, 0x45, 0x1c, 0xce, 0x89, 0x0f, 0x53, 0xe5, 0x83, 0x22, 0x9a, 0xd0, 0xe2, 0xee, 0x64,
        0x56, 0x11, 0xfa, 0x99, 0x36, 0xdf, 0xa4, 0x53, 0x06, 0xec, 0x18, 0x06, 0x67, 0x74, 0xaa, 0x24,
    };

    struct aws_byte_cursor pub_x = aws_byte_cursor_from_array(x, sizeof(x));

    uint8_t y[] = {
        0xb8, 0x3c, 0xa4, 0x12, 0x6c, 0xfc, 0x4c, 0x4d, 0x1d, 0x18, 0xa4, 0xb6, 0xc2, 0x1c, 0x7f, 0x69,
        0x9d, 0x51, 0x23, 0xdd, 0x9c, 0x24, 0xf6, 0x6f, 0x83, 0x38, 0x46, 0xee, 0xb5, 0x82, 0x96, 0x19,
        0x6b, 0x42, 0xec, 0x06, 0x42, 0x5d, 0xb5, 0xb7, 0x0a, 0x4b, 0x81, 0xb7, 0xfc, 0xf7, 0x05, 0xa0,
    };

    struct aws_byte_cursor pub_y = aws_byte_cursor_from_array(y, sizeof(y));

    return s_test_known_signing_value(allocator, AWS_CAL_ECDSA_P384, private_key, pub_x, pub_y);
}

AWS_TEST_CASE(ecdsa_p384_test_known_signing_value, s_ecdsa_p384_test_known_signing_value_fn)

static int s_test_key_gen(struct aws_allocator *allocator, enum aws_ecc_curve_name curve_name) {
    struct aws_ecc_key_pair *key_pair = aws_ecc_key_pair_new_generate_random(allocator, curve_name);

    struct aws_byte_cursor pub_x;
    struct aws_byte_cursor pub_y;

    aws_ecc_key_pair_get_public_key(key_pair, &pub_x, &pub_y);
    ASSERT_TRUE(pub_x.len > 0);
    ASSERT_TRUE(pub_y.len > 0);

    struct aws_byte_cursor priv_d;
    aws_ecc_key_pair_get_private_key(key_pair, &priv_d);
    ASSERT_TRUE(priv_d.len > 0);

    uint8_t message[] = {
        0x59, 0x05, 0x23, 0x88, 0x77, 0xc7, 0x74, 0x21, 0xf7, 0x3e, 0x43, 0xee, 0x3d, 0xa6, 0xf2, 0xd9,
        0xe2, 0xcc, 0xad, 0x5f, 0xc9, 0x42, 0xdc, 0xec, 0x0c, 0xbd, 0x25, 0x48, 0x29, 0x35, 0xfa, 0xaf,
        0x41, 0x69, 0x83, 0xfe, 0x16, 0x5b, 0x1a, 0x04, 0x5e, 0xe2, 0xbc, 0xd2, 0xe6, 0xdc, 0xa3, 0xbd,
        0xf4, 0x6c, 0x43, 0x10, 0xa7, 0x46, 0x1f, 0x9a, 0x37, 0x96, 0x0c, 0xa6, 0x72, 0xd3, 0xfe, 0xb5,
        0x47, 0x3e, 0x25, 0x36, 0x05, 0xfb, 0x1d, 0xdf, 0xd2, 0x80, 0x65, 0xb5, 0x3c, 0xb5, 0x85, 0x8a,
        0x8a, 0xd2, 0x81, 0x75, 0xbf, 0x9b, 0xd3, 0x86, 0xa5, 0xe4, 0x71, 0xea, 0x7a, 0x65, 0xc1, 0x7c,
        0xc9, 0x34, 0xa9, 0xd7, 0x91, 0xe9, 0x14, 0x91, 0xeb, 0x37, 0x54, 0xd0, 0x37, 0x99, 0x79, 0x0f,
        0xe2, 0xd3, 0x08, 0xd1, 0x61, 0x46, 0xd5, 0xc9, 0xb0, 0xd0, 0xde, 0xbd, 0x97, 0xd7, 0x9c, 0xe8,
    };

    struct aws_byte_cursor message_input = aws_byte_cursor_from_array(message, sizeof(message));
    uint8_t hash[AWS_SHA256_LEN];
    AWS_ZERO_ARRAY(hash);
    struct aws_byte_buf hash_value = aws_byte_buf_from_empty_array(hash, sizeof(hash));
    aws_sha256_compute(allocator, &message_input, &hash_value, 0);

    size_t signature_size = aws_ecc_key_pair_signature_length(key_pair);

    struct aws_byte_buf signature_buf;
    AWS_ZERO_STRUCT(signature_buf);
    aws_byte_buf_init(&signature_buf, allocator, signature_size);

    struct aws_byte_cursor hash_cur = aws_byte_cursor_from_buf(&hash_value);
    ASSERT_SUCCESS(aws_ecc_key_pair_sign_message(key_pair, &hash_cur, &signature_buf));

    struct aws_byte_cursor signature_cur = aws_byte_cursor_from_buf(&signature_buf);
    ASSERT_SUCCESS(aws_ecc_key_pair_verify_signature(key_pair, &hash_cur, &signature_cur));

    aws_byte_buf_clean_up(&signature_buf);
    aws_ecc_key_pair_destroy(key_pair);

    return AWS_OP_SUCCESS;
}

static int s_ecdsa_p256_test_key_gen_fn(struct aws_allocator *allocator, void *ctx) {
    (void)ctx;

    return s_test_key_gen(allocator, AWS_CAL_ECDSA_P256);
}

AWS_TEST_CASE(ecdsa_p256_test_key_gen, s_ecdsa_p256_test_key_gen_fn)

static int s_ecdsa_p384_test_key_gen_fn(struct aws_allocator *allocator, void *ctx) {
    (void)ctx;

    return s_test_key_gen(allocator, AWS_CAL_ECDSA_P256);
}

AWS_TEST_CASE(ecdsa_p384_test_key_gen, s_ecdsa_p384_test_key_gen_fn)

static int s_test_key_gen_export(struct aws_allocator *allocator, enum aws_ecc_curve_name curve_name) {

    struct aws_ecc_key_pair *key_pair = aws_ecc_key_pair_new_generate_random(allocator, curve_name);

    struct aws_byte_cursor pub_x;
    struct aws_byte_cursor pub_y;

    aws_ecc_key_pair_get_public_key(key_pair, &pub_x, &pub_y);
    ASSERT_TRUE(pub_x.len > 0);
    ASSERT_TRUE(pub_y.len > 0);

    struct aws_byte_cursor priv_d;
    aws_ecc_key_pair_get_private_key(key_pair, &priv_d);
    ASSERT_TRUE(priv_d.len > 0);

    struct aws_ecc_key_pair *signing_key = aws_ecc_key_pair_new_from_private_key(allocator, curve_name, &priv_d);
    ASSERT_NOT_NULL(signing_key);

    uint8_t message[] = {
        0x59, 0x05, 0x23, 0x88, 0x77, 0xc7, 0x74, 0x21, 0xf7, 0x3e, 0x43, 0xee, 0x3d, 0xa6, 0xf2, 0xd9,
        0xe2, 0xcc, 0xad, 0x5f, 0xc9, 0x42, 0xdc, 0xec, 0x0c, 0xbd, 0x25, 0x48, 0x29, 0x35, 0xfa, 0xaf,
        0x41, 0x69, 0x83, 0xfe, 0x16, 0x5b, 0x1a, 0x04, 0x5e, 0xe2, 0xbc, 0xd2, 0xe6, 0xdc, 0xa3, 0xbd,
        0xf4, 0x6c, 0x43, 0x10, 0xa7, 0x46, 0x1f, 0x9a, 0x37, 0x96, 0x0c, 0xa6, 0x72, 0xd3, 0xfe, 0xb5,
        0x47, 0x3e, 0x25, 0x36, 0x05, 0xfb, 0x1d, 0xdf, 0xd2, 0x80, 0x65, 0xb5, 0x3c, 0xb5, 0x85, 0x8a,
        0x8a, 0xd2, 0x81, 0x75, 0xbf, 0x9b, 0xd3, 0x86, 0xa5, 0xe4, 0x71, 0xea, 0x7a, 0x65, 0xc1, 0x7c,
        0xc9, 0x34, 0xa9, 0xd7, 0x91, 0xe9, 0x14, 0x91, 0xeb, 0x37, 0x54, 0xd0, 0x37, 0x99, 0x79, 0x0f,
        0xe2, 0xd3, 0x08, 0xd1, 0x61, 0x46, 0xd5, 0xc9, 0xb0, 0xd0, 0xde, 0xbd, 0x97, 0xd7, 0x9c, 0xe8,
    };

    struct aws_byte_cursor message_input = aws_byte_cursor_from_array(message, sizeof(message));
    uint8_t hash[AWS_SHA256_LEN];
    AWS_ZERO_ARRAY(hash);
    struct aws_byte_buf hash_value = aws_byte_buf_from_empty_array(hash, sizeof(hash));
    aws_sha256_compute(allocator, &message_input, &hash_value, 0);

    size_t signature_size = aws_ecc_key_pair_signature_length(key_pair);

    struct aws_byte_buf signature_buf;
    AWS_ZERO_STRUCT(signature_buf);
    aws_byte_buf_init(&signature_buf, allocator, signature_size);

    struct aws_byte_cursor hash_cur = aws_byte_cursor_from_buf(&hash_value);
    ASSERT_SUCCESS(aws_ecc_key_pair_sign_message(signing_key, &hash_cur, &signature_buf));

    struct aws_ecc_key_pair *verifying_key =
        aws_ecc_key_pair_new_from_public_key(allocator, curve_name, &pub_x, &pub_y);
    ASSERT_NOT_NULL(verifying_key);

    struct aws_byte_cursor signature_cur = aws_byte_cursor_from_buf(&signature_buf);
    ASSERT_SUCCESS(aws_ecc_key_pair_verify_signature(verifying_key, &hash_cur, &signature_cur));

    aws_byte_buf_clean_up(&signature_buf);
    aws_ecc_key_pair_destroy(key_pair);
    aws_ecc_key_pair_destroy(signing_key);
    aws_ecc_key_pair_destroy(verifying_key);

    return AWS_OP_SUCCESS;
}

static int s_ecdsa_p256_test_key_gen_export_fn(struct aws_allocator *allocator, void *ctx) {
    (void)ctx;

    return s_test_key_gen_export(allocator, AWS_CAL_ECDSA_P256);
}

AWS_TEST_CASE(ecdsa_p256_test_key_gen_export, s_ecdsa_p256_test_key_gen_export_fn)

static int s_ecdsa_p384_test_key_gen_export_fn(struct aws_allocator *allocator, void *ctx) {
    (void)ctx;

    return s_test_key_gen_export(allocator, AWS_CAL_ECDSA_P256);
}

AWS_TEST_CASE(ecdsa_p384_test_key_gen_export, s_ecdsa_p384_test_key_gen_export_fn)

static int s_ecdsa_test_import_asn1_key_pair(
    struct aws_allocator *allocator,
    struct aws_byte_cursor asn1_cur,
    enum aws_ecc_curve_name expected_curve_name) {

    struct aws_ecc_key_pair *imported_key = aws_ecc_key_pair_new_from_asn1(allocator, &asn1_cur);
    ASSERT_NOT_NULL(imported_key);

    ASSERT_INT_EQUALS(expected_curve_name, imported_key->curve_name);

    uint8_t message[] = {
        0x59, 0x05, 0x23, 0x88, 0x77, 0xc7, 0x74, 0x21, 0xf7, 0x3e, 0x43, 0xee, 0x3d, 0xa6, 0xf2, 0xd9,
        0xe2, 0xcc, 0xad, 0x5f, 0xc9, 0x42, 0xdc, 0xec, 0x0c, 0xbd, 0x25, 0x48, 0x29, 0x35, 0xfa, 0xaf,
        0x41, 0x69, 0x83, 0xfe, 0x16, 0x5b, 0x1a, 0x04, 0x5e, 0xe2, 0xbc, 0xd2, 0xe6, 0xdc, 0xa3, 0xbd,
        0xf4, 0x6c, 0x43, 0x10, 0xa7, 0x46, 0x1f, 0x9a, 0x37, 0x96, 0x0c, 0xa6, 0x72, 0xd3, 0xfe, 0xb5,
        0x47, 0x3e, 0x25, 0x36, 0x05, 0xfb, 0x1d, 0xdf, 0xd2, 0x80, 0x65, 0xb5, 0x3c, 0xb5, 0x85, 0x8a,
        0x8a, 0xd2, 0x81, 0x75, 0xbf, 0x9b, 0xd3, 0x86, 0xa5, 0xe4, 0x71, 0xea, 0x7a, 0x65, 0xc1, 0x7c,
        0xc9, 0x34, 0xa9, 0xd7, 0x91, 0xe9, 0x14, 0x91, 0xeb, 0x37, 0x54, 0xd0, 0x37, 0x99, 0x79, 0x0f,
        0xe2, 0xd3, 0x08, 0xd1, 0x61, 0x46, 0xd5, 0xc9, 0xb0, 0xd0, 0xde, 0xbd, 0x97, 0xd7, 0x9c, 0xe8,
    };
    struct aws_byte_cursor message_input = aws_byte_cursor_from_array(message, sizeof(message));
    uint8_t hash[AWS_SHA256_LEN];
    AWS_ZERO_ARRAY(hash);
    struct aws_byte_buf hash_value = aws_byte_buf_from_empty_array(hash, sizeof(hash));
    aws_sha256_compute(allocator, &message_input, &hash_value, 0);

    size_t signature_size = aws_ecc_key_pair_signature_length(imported_key);

    struct aws_byte_buf signature_buf;
    AWS_ZERO_STRUCT(signature_buf);
    aws_byte_buf_init(&signature_buf, allocator, signature_size);

    struct aws_byte_cursor hash_cur = aws_byte_cursor_from_buf(&hash_value);
    ASSERT_SUCCESS(aws_ecc_key_pair_sign_message(imported_key, &hash_cur, &signature_buf));

    struct aws_byte_cursor signature_cur = aws_byte_cursor_from_buf(&signature_buf);
    ASSERT_SUCCESS(aws_ecc_key_pair_verify_signature(imported_key, &hash_cur, &signature_cur));

    aws_byte_buf_clean_up(&signature_buf);
    aws_ecc_key_pair_destroy(imported_key);

    return AWS_OP_SUCCESS;
}

static int s_ecdsa_p256_test_import_asn1_key_pair_fn(struct aws_allocator *allocator, void *ctx) {
    (void)ctx;

    uint8_t asn1_encoded_key_raw[] = {
        0x30, 0x77, 0x02, 0x01, 0x01, 0x04, 0x20, 0x78, 0xed, 0xed, 0xcf, 0x95, 0x9e, 0x42, 0x24, 0x37, 0xa4, 0x56,
        0xed, 0x08, 0x19, 0x3c, 0x53, 0x4b, 0x6f, 0xff, 0x40, 0x64, 0x48, 0x6a, 0x49, 0x86, 0x0c, 0xb7, 0x0a, 0xe5,
        0x2d, 0xbd, 0xd6, 0xa0, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0xa1, 0x44, 0x03,
        0x42, 0x00, 0x04, 0xbf, 0x61, 0x63, 0x46, 0x93, 0x2d, 0x00, 0x33, 0x19, 0xe3, 0x3a, 0x19, 0xc6, 0xc8, 0x55,
        0xf5, 0xc8, 0x44, 0x91, 0xe9, 0x9b, 0x83, 0x36, 0x67, 0x5d, 0x25, 0x0d, 0x7b, 0xe0, 0xc0, 0xf1, 0xd2, 0xaa,
        0x5c, 0xdf, 0xfb, 0xa9, 0x37, 0x19, 0x8d, 0x82, 0x47, 0x28, 0x88, 0xbe, 0x46, 0x7f, 0x3c, 0xcd, 0x41, 0xaa,
        0x08, 0x9a, 0x37, 0x0d, 0x61, 0x7f, 0x5f, 0xeb, 0x9f, 0x55, 0xf7, 0x54, 0xda, 0x0a,
    };

    struct aws_byte_cursor asn1_encoded_key =
        aws_byte_cursor_from_array(asn1_encoded_key_raw, sizeof(asn1_encoded_key_raw));

    return s_ecdsa_test_import_asn1_key_pair(allocator, asn1_encoded_key, AWS_CAL_ECDSA_P256);
}

AWS_TEST_CASE(ecdsa_p256_test_import_asn1_key_pair, s_ecdsa_p256_test_import_asn1_key_pair_fn)

static int s_ecdsa_p384_test_import_asn1_key_pair_fn(struct aws_allocator *allocator, void *ctx) {
    (void)ctx;

    uint8_t asn1_encoded_key_raw[] = {
        0x30, 0x81, 0xa4, 0x02, 0x01, 0x01, 0x04, 0x30, 0xa4, 0x4e, 0x2c, 0xf8, 0x6a, 0xfd, 0x42, 0x0e, 0xd4,
        0xbb, 0x2d, 0x08, 0xe2, 0x35, 0xe7, 0xb2, 0xc7, 0x87, 0x37, 0xbc, 0x92, 0xc2, 0x9a, 0x84, 0x39, 0x99,
        0x24, 0xe3, 0xa3, 0x01, 0x8c, 0xa0, 0xc1, 0x34, 0xd7, 0x8d, 0x86, 0xa1, 0x8c, 0xe1, 0xe4, 0x3e, 0xd1,
        0xe3, 0xff, 0x8b, 0xa4, 0x1d, 0xa0, 0x07, 0x06, 0x05, 0x2b, 0x81, 0x04, 0x00, 0x22, 0xa1, 0x64, 0x03,
        0x62, 0x00, 0x04, 0x89, 0x64, 0x99, 0x1e, 0x1e, 0xa3, 0x6e, 0x30, 0x5e, 0xb1, 0x00, 0xef, 0x51, 0x0e,
        0x78, 0xc6, 0x7a, 0x2f, 0x1d, 0x21, 0x65, 0xe2, 0x68, 0xfa, 0x22, 0x5f, 0x1c, 0x8e, 0x00, 0xdc, 0x74,
        0xa6, 0x97, 0x7d, 0x73, 0xb3, 0x05, 0x00, 0xfd, 0xf5, 0x52, 0x85, 0xd8, 0x81, 0x62, 0x6c, 0x0a, 0x04,
        0xf4, 0xaa, 0x39, 0xe0, 0x2b, 0x05, 0x29, 0xd1, 0x70, 0x8a, 0x42, 0x44, 0x9a, 0xe7, 0xed, 0xec, 0x3a,
        0x52, 0x06, 0x24, 0x67, 0x35, 0x27, 0x6b, 0x80, 0x7c, 0xda, 0xc5, 0xe9, 0x97, 0xfa, 0x1d, 0xd4, 0x0c,
        0x27, 0x54, 0xed, 0x97, 0xe6, 0xbd, 0xe0, 0xaf, 0xff, 0xaa, 0xb9, 0x63, 0xf3, 0x21, 0x0a,
    };

    struct aws_byte_cursor asn1_encoded_key =
        aws_byte_cursor_from_array(asn1_encoded_key_raw, sizeof(asn1_encoded_key_raw));

    return s_ecdsa_test_import_asn1_key_pair(allocator, asn1_encoded_key, AWS_CAL_ECDSA_P384);
}

AWS_TEST_CASE(ecdsa_p384_test_import_asn1_key_pair, s_ecdsa_p384_test_import_asn1_key_pair_fn)

static int s_ecdsa_test_import_asn1_key_pair_public_only_fn(struct aws_allocator *allocator, void *ctx) {
    (void)ctx;

    uint8_t asn1_encoded_full_key_raw[] = {
        0x30, 0x77, 0x02, 0x01, 0x01, 0x04, 0x20, 0x99, 0x16, 0x2a, 0x5b, 0x4e, 0x63, 0x86, 0x4c, 0x5f, 0x8e, 0x37,
        0xf7, 0x2b, 0xbd, 0x97, 0x1d, 0x5c, 0x68, 0x80, 0x18, 0xc3, 0x91, 0x0f, 0xb3, 0xc3, 0xf9, 0x3a, 0xc9, 0x7a,
        0x4b, 0xa3, 0xf6, 0xa0, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0xa1, 0x44, 0x03,
        0x42, 0x00, 0x04, 0xec, 0x6c, 0xd7, 0x4b, 0xdc, 0x33, 0xc2, 0x56, 0x32, 0xad, 0x52, 0x56, 0xac, 0xf5, 0xf0,
        0xe6, 0x28, 0x99, 0x84, 0x83, 0xaf, 0x73, 0x6f, 0xfe, 0xd7, 0x83, 0x3b, 0x42, 0x81, 0x5d, 0x2e, 0xe0, 0xdb,
        0xf6, 0xac, 0xa4, 0xc6, 0x16, 0x7e, 0x3e, 0xe0, 0xff, 0x7b, 0x43, 0xe8, 0xa1, 0x36, 0x50, 0x92, 0x83, 0x06,
        0x94, 0xb3, 0xd4, 0x93, 0x06, 0xde, 0x63, 0x8a, 0xa1, 0x1c, 0x3f, 0xb2, 0x57, 0x0a,
    };

    uint8_t asn1_encoded_pub_key_raw[] = {
        0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x06, 0x08, 0x2a,
        0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0xec, 0x6c, 0xd7, 0x4b, 0xdc,
        0x33, 0xc2, 0x56, 0x32, 0xad, 0x52, 0x56, 0xac, 0xf5, 0xf0, 0xe6, 0x28, 0x99, 0x84, 0x83, 0xaf,
        0x73, 0x6f, 0xfe, 0xd7, 0x83, 0x3b, 0x42, 0x81, 0x5d, 0x2e, 0xe0, 0xdb, 0xf6, 0xac, 0xa4, 0xc6,
        0x16, 0x7e, 0x3e, 0xe0, 0xff, 0x7b, 0x43, 0xe8, 0xa1, 0x36, 0x50, 0x92, 0x83, 0x06, 0x94, 0xb3,
        0xd4, 0x93, 0x06, 0xde, 0x63, 0x8a, 0xa1, 0x1c, 0x3f, 0xb2, 0x57, 0x0a,
    };

    struct aws_byte_cursor full_key_asn1 =
        aws_byte_cursor_from_array(asn1_encoded_full_key_raw, sizeof(asn1_encoded_full_key_raw));
    struct aws_byte_cursor pub_key_asn1 =
        aws_byte_cursor_from_array(asn1_encoded_pub_key_raw, sizeof(asn1_encoded_pub_key_raw));

    struct aws_ecc_key_pair *signing_key = aws_ecc_key_pair_new_from_asn1(allocator, &full_key_asn1);
    ASSERT_NOT_NULL(signing_key);

    struct aws_ecc_key_pair *verifying_key = aws_ecc_key_pair_new_from_asn1(allocator, &pub_key_asn1);
    ASSERT_NOT_NULL(verifying_key);

    uint8_t message[] = {
        0x59, 0x05, 0x23, 0x88, 0x77, 0xc7, 0x74, 0x21, 0xf7, 0x3e, 0x43, 0xee, 0x3d, 0xa6, 0xf2, 0xd9,
        0xe2, 0xcc, 0xad, 0x5f, 0xc9, 0x42, 0xdc, 0xec, 0x0c, 0xbd, 0x25, 0x48, 0x29, 0x35, 0xfa, 0xaf,
        0x41, 0x69, 0x83, 0xfe, 0x16, 0x5b, 0x1a, 0x04, 0x5e, 0xe2, 0xbc, 0xd2, 0xe6, 0xdc, 0xa3, 0xbd,
        0xf4, 0x6c, 0x43, 0x10, 0xa7, 0x46, 0x1f, 0x9a, 0x37, 0x96, 0x0c, 0xa6, 0x72, 0xd3, 0xfe, 0xb5,
        0x47, 0x3e, 0x25, 0x36, 0x05, 0xfb, 0x1d, 0xdf, 0xd2, 0x80, 0x65, 0xb5, 0x3c, 0xb5, 0x85, 0x8a,
        0x8a, 0xd2, 0x81, 0x75, 0xbf, 0x9b, 0xd3, 0x86, 0xa5, 0xe4, 0x71, 0xea, 0x7a, 0x65, 0xc1, 0x7c,
        0xc9, 0x34, 0xa9, 0xd7, 0x91, 0xe9, 0x14, 0x91, 0xeb, 0x37, 0x54, 0xd0, 0x37, 0x99, 0x79, 0x0f,
        0xe2, 0xd3, 0x08, 0xd1, 0x61, 0x46, 0xd5, 0xc9, 0xb0, 0xd0, 0xde, 0xbd, 0x97, 0xd7, 0x9c, 0xe8,
    };

    struct aws_byte_cursor message_input = aws_byte_cursor_from_array(message, sizeof(message));
    uint8_t hash[AWS_SHA256_LEN];
    AWS_ZERO_ARRAY(hash);
    struct aws_byte_buf hash_value = aws_byte_buf_from_empty_array(hash, sizeof(hash));
    aws_sha256_compute(allocator, &message_input, &hash_value, 0);

    size_t signature_size = aws_ecc_key_pair_signature_length(signing_key);

    struct aws_byte_buf signature_buf;
    AWS_ZERO_STRUCT(signature_buf);
    aws_byte_buf_init(&signature_buf, allocator, signature_size);

    struct aws_byte_cursor hash_cur = aws_byte_cursor_from_buf(&hash_value);
    ASSERT_SUCCESS(aws_ecc_key_pair_sign_message(signing_key, &hash_cur, &signature_buf));

    struct aws_byte_cursor signature_cur = aws_byte_cursor_from_buf(&signature_buf);
    ASSERT_SUCCESS(aws_ecc_key_pair_verify_signature(verifying_key, &hash_cur, &signature_cur));

    aws_byte_buf_clean_up(&signature_buf);
    aws_ecc_key_pair_destroy(verifying_key);
    aws_ecc_key_pair_destroy(signing_key);

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(ecdsa_test_import_asn1_key_pair_public_only, s_ecdsa_test_import_asn1_key_pair_public_only_fn)

/* this test exists because we have to manually handle signature encoding/decoding on windows.
   this takes an encoded signature and makes sure we decode and verify it properly. How do we know
   we encode properly b.t.w? Well we have tests that verify signatures we generated, so we already know
   that anything we signed can be decoded. What we don't have proven is that we're not just symetrically
   wrong. So, let's take the format we know signatures must be in ASN.1 DER encoded, and make sure we can
   verify it. Since we KNOW the signing and verifying code is symetric, verifying the verification side should
   prove our encoding/decoding code is correct to the spec. */
static int s_ecdsa_test_signature_format_fn(struct aws_allocator *allocator, void *ctx) {
    (void)ctx;

    uint8_t asn1_encoded_signature_raw[] = {
        0x30, 0x45, 0x02, 0x21, 0x00, 0xd7, 0xc5, 0xb9, 0x9e, 0x0b, 0xb1, 0x1a, 0x1f, 0x32, 0xda, 0x66, 0xe0, 0xff,
        0x59, 0xb7, 0x8a, 0x5e, 0xb3, 0x94, 0x9c, 0x23, 0xb3, 0xfc, 0x1f, 0x18, 0xcc, 0xf6, 0x61, 0x67, 0x8b, 0xf1,
        0xc1, 0x02, 0x20, 0x26, 0x4d, 0x8b, 0x7c, 0xaa, 0x52, 0x4c, 0xc0, 0x2e, 0x5f, 0xf6, 0x7e, 0x24, 0x82, 0xe5,
        0xfb, 0xcb, 0xc7, 0x9b, 0x83, 0x0d, 0x19, 0x7e, 0x7a, 0x40, 0x37, 0x87, 0xdd, 0x1c, 0x93, 0x13, 0xc4,
    };

    uint8_t x[] = {
        0x1c, 0xcb, 0xe9, 0x1c, 0x07, 0x5f, 0xc7, 0xf4, 0xf0, 0x33, 0xbf, 0xa2, 0x48, 0xdb, 0x8f, 0xcc,
        0xd3, 0x56, 0x5d, 0xe9, 0x4b, 0xbf, 0xb1, 0x2f, 0x3c, 0x59, 0xff, 0x46, 0xc2, 0x71, 0xbf, 0x83,
    };

    uint8_t y[] = {
        0xce, 0x40, 0x14, 0xc6, 0x88, 0x11, 0xf9, 0xa2, 0x1a, 0x1f, 0xdb, 0x2c, 0x0e, 0x61, 0x13, 0xe0,
        0x6d, 0xb7, 0xca, 0x93, 0xb7, 0x40, 0x4e, 0x78, 0xdc, 0x7c, 0xcd, 0x5c, 0xa8, 0x9a, 0x4c, 0xa9,
    };

    struct aws_byte_cursor pub_x = aws_byte_cursor_from_array(x, sizeof(x));
    struct aws_byte_cursor pub_y = aws_byte_cursor_from_array(y, sizeof(y));
    struct aws_ecc_key_pair *verifying_key =
        aws_ecc_key_pair_new_from_public_key(allocator, AWS_CAL_ECDSA_P256, &pub_x, &pub_y);
    ASSERT_NOT_NULL(verifying_key);

    uint8_t message[] = {
        0x59, 0x05, 0x23, 0x88, 0x77, 0xc7, 0x74, 0x21, 0xf7, 0x3e, 0x43, 0xee, 0x3d, 0xa6, 0xf2, 0xd9,
        0xe2, 0xcc, 0xad, 0x5f, 0xc9, 0x42, 0xdc, 0xec, 0x0c, 0xbd, 0x25, 0x48, 0x29, 0x35, 0xfa, 0xaf,
        0x41, 0x69, 0x83, 0xfe, 0x16, 0x5b, 0x1a, 0x04, 0x5e, 0xe2, 0xbc, 0xd2, 0xe6, 0xdc, 0xa3, 0xbd,
        0xf4, 0x6c, 0x43, 0x10, 0xa7, 0x46, 0x1f, 0x9a, 0x37, 0x96, 0x0c, 0xa6, 0x72, 0xd3, 0xfe, 0xb5,
        0x47, 0x3e, 0x25, 0x36, 0x05, 0xfb, 0x1d, 0xdf, 0xd2, 0x80, 0x65, 0xb5, 0x3c, 0xb5, 0x85, 0x8a,
        0x8a, 0xd2, 0x81, 0x75, 0xbf, 0x9b, 0xd3, 0x86, 0xa5, 0xe4, 0x71, 0xea, 0x7a, 0x65, 0xc1, 0x7c,
        0xc9, 0x34, 0xa9, 0xd7, 0x91, 0xe9, 0x14, 0x91, 0xeb, 0x37, 0x54, 0xd0, 0x37, 0x99, 0x79, 0x0f,
        0xe2, 0xd3, 0x08, 0xd1, 0x61, 0x46, 0xd5, 0xc9, 0xb0, 0xd0, 0xde, 0xbd, 0x97, 0xd7, 0x9c, 0xe8,
    };

    struct aws_byte_cursor message_input = aws_byte_cursor_from_array(message, sizeof(message));
    uint8_t hash[AWS_SHA256_LEN];
    AWS_ZERO_ARRAY(hash);
    struct aws_byte_buf hash_value = aws_byte_buf_from_empty_array(hash, sizeof(hash));
    aws_sha256_compute(allocator, &message_input, &hash_value, 0);
    struct aws_byte_cursor hash_cur = aws_byte_cursor_from_buf(&hash_value);
    struct aws_byte_buf signature_buf =
        aws_byte_buf_from_array(asn1_encoded_signature_raw, sizeof(asn1_encoded_signature_raw));

    struct aws_byte_cursor signature_cur = aws_byte_cursor_from_buf(&signature_buf);
    ASSERT_SUCCESS(aws_ecc_key_pair_verify_signature(verifying_key, &hash_cur, &signature_cur));

    aws_ecc_key_pair_destroy(verifying_key);

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(ecdsa_test_signature_format, s_ecdsa_test_signature_format_fn)
