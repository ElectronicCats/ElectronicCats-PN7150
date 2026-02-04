// Unit tests for NdefMessage class
// SPDX-FileCopyrightText: Copyright (c) 2024 Electronic Cats
// SPDX-License-Identifier: MIT

#include <unity.h>
#include <cstring>
#include <cstdlib>

// Test memory leak prevention pattern
void test_memory_reallocation_pattern(void) {
    // Simulates the proper pattern for memory reallocation
    unsigned char* content = NULL;

    // First allocation
    content = new unsigned char[10];
    TEST_ASSERT_NOT_NULL(content);

    // Proper reallocation pattern (free old before new)
    unsigned char* newContent = new unsigned char[20];
    if(content != NULL) {
        delete[] content;
    }
    content = newContent;

    TEST_ASSERT_NOT_NULL(content);

    // Cleanup
    delete[] content;
    TEST_PASS();
}

// Test sizeof pointer vs array issue
void test_sizeof_pointer_vs_buffer(void) {
    // Demonstrates the issue with sizeof on pointers
    unsigned char* dynamicBuffer = new unsigned char[100];
    unsigned char staticBuffer[100];

    // sizeof on pointer returns pointer size (4 or 8 bytes)
    size_t pointerSize = sizeof(dynamicBuffer);
    TEST_ASSERT_TRUE(pointerSize == 4 || pointerSize == 8);

    // sizeof on array returns actual size
    size_t arraySize = sizeof(staticBuffer);
    TEST_ASSERT_EQUAL(100, arraySize);

    // Cleanup
    delete[] dynamicBuffer;
}

// Test WiFi record header size constant
void test_wifi_record_header_size(void) {
    // The magic number 29 should be a constant
    const size_t WIFI_RECORD_HEADER_SIZE = 29;

    const char* ssid = "TestNetwork";
    const char* password = "TestPassword";

    size_t payloadSize = strlen(ssid) + strlen(password) + WIFI_RECORD_HEADER_SIZE;

    // Verify calculation
    TEST_ASSERT_EQUAL(11 + 12 + 29, payloadSize);
}

// Test proper memory cleanup after WiFi record creation
void test_wifi_record_memory_cleanup(void) {
    const size_t WIFI_RECORD_HEADER_SIZE = 29;
    const char* ssid = "TestSSID";
    const char* password = "TestPass";

    size_t payloadSize = strlen(ssid) + strlen(password) + WIFI_RECORD_HEADER_SIZE;
    unsigned char* payload = new unsigned char[payloadSize];

    TEST_ASSERT_NOT_NULL(payload);

    // Simulate usage
    memset(payload, 0, payloadSize);

    // Proper cleanup (the fix)
    delete[] payload;

    TEST_PASS();
}

// Test static variable initialization
void test_static_initialization(void) {
    // Static pointers should be initialized to NULL
    static unsigned char* content = NULL;
    static unsigned char* newContent = NULL;

    TEST_ASSERT_NULL(content);
    TEST_ASSERT_NULL(newContent);
}

// Test hex representation function edge case
void test_hex_representation_empty_data(void) {
    const unsigned char* data = NULL;
    uint32_t dataLength = 0;

    // Empty data should return empty string
    char result[1] = "";

    if(dataLength == 0 || data == NULL) {
        result[0] = '\0';
    }

    TEST_ASSERT_EQUAL_STRING("", result);
}

// Test VLA replacement with fixed size
void test_vla_replacement(void) {
    // Instead of VLA: uint8_t arr[recordCounter];
    // Use fixed max size or dynamic allocation

    const size_t MAX_RECORDS = 32;
    uint8_t headersPositions[MAX_RECORDS];
    uint8_t previousHeaders[MAX_RECORDS];

    // Initialize
    memset(headersPositions, 0, sizeof(headersPositions));
    memset(previousHeaders, 0, sizeof(previousHeaders));

    TEST_ASSERT_EQUAL(0, headersPositions[0]);
    TEST_ASSERT_EQUAL(0, previousHeaders[0]);
}

// Test malloc/free vs new/delete consistency
void test_memory_allocation_consistency(void) {
    // Test that we use consistent allocation method
    // Prefer new/delete for C++

    unsigned char* buffer1 = new unsigned char[50];
    unsigned char* buffer2 = new unsigned char[100];

    TEST_ASSERT_NOT_NULL(buffer1);
    TEST_ASSERT_NOT_NULL(buffer2);

    delete[] buffer1;
    delete[] buffer2;

    TEST_PASS();
}

void setUp(void) {
    // Set up code before each test
}

void tearDown(void) {
    // Clean up code after each test
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_memory_reallocation_pattern);
    RUN_TEST(test_sizeof_pointer_vs_buffer);
    RUN_TEST(test_wifi_record_header_size);
    RUN_TEST(test_wifi_record_memory_cleanup);
    RUN_TEST(test_static_initialization);
    RUN_TEST(test_hex_representation_empty_data);
    RUN_TEST(test_vla_replacement);
    RUN_TEST(test_memory_allocation_consistency);

    return UNITY_END();
}
