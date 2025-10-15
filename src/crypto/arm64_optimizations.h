// ARM64-specific optimizations for Fuego
// Copyright (c) 2024 Fuego Developers

#ifndef ARM64_OPTIMIZATIONS_H
#define ARM64_OPTIMIZATIONS_H

// Only include ARM64 optimizations when building for ARM64
#if defined(__aarch64__) && defined(__ARM_NEON)

#include <arm_neon.h>
#include <arm_acle.h>

// ARM64-specific memory prefetching
#define ARM64_PREFETCH_READ(addr) __builtin_prefetch((addr), 0, 3)
#define ARM64_PREFETCH_WRITE(addr) __builtin_prefetch((addr), 1, 3)

// ARM64-specific cache line size (typically 64 bytes)
#define ARM64_CACHE_LINE_SIZE 64

// Optimized memory operations for ARM64
static inline void arm64_memcpy_optimized(void* dest, const void* src, size_t n) {
    if (n < 16) {
        // Use scalar operations for small copies
        uint8_t* d = (uint8_t*)dest;
        const uint8_t* s = (const uint8_t*)src;
        for (size_t i = 0; i < n; i++) {
            d[i] = s[i];
        }
        return;
    }
    
    // Use NEON for larger copies
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    
    // Align destination to 16-byte boundary
    while (((uintptr_t)d & 15) && n > 0) {
        *d++ = *s++;
        n--;
    }
    
    // Use NEON for bulk copy
    while (n >= 16) {
        uint8x16_t data = vld1q_u8(s);
        vst1q_u8(d, data);
        d += 16;
        s += 16;
        n -= 16;
    }
    
    // Handle remaining bytes
    while (n > 0) {
        *d++ = *s++;
        n--;
    }
}

// ARM64-optimized hash operations
static inline void arm64_hash_block_neon(const uint8_t* input, uint8_t* output, size_t blocks) {
    // Prefetch input data
    ARM64_PREFETCH_READ(input);
    ARM64_PREFETCH_READ(input + ARM64_CACHE_LINE_SIZE);
    
    // Process multiple blocks in parallel using NEON
    for (size_t i = 0; i < blocks; i += 4) {
        // Load 4 blocks at once
        uint8x16_t block0 = vld1q_u8(input + i * 16);
        uint8x16_t block1 = vld1q_u8(input + (i + 1) * 16);
        uint8x16_t block2 = vld1q_u8(input + (i + 2) * 16);
        uint8x16_t block3 = vld1q_u8(input + (i + 3) * 16);
        
        // Process blocks (simplified example)
        // In real implementation, this would contain the actual hash algorithm
        
        // Store results
        vst1q_u8(output + i * 16, block0);
        vst1q_u8(output + (i + 1) * 16, block1);
        vst1q_u8(output + (i + 2) * 16, block2);
        vst1q_u8(output + (i + 3) * 16, block3);
    }
}

// ARM64-optimized AES operations using crypto extensions
static inline void arm64_aes_encrypt_neon(const uint8_t* input, uint8_t* output, const uint8_t* key) {
    uint8x16_t data = vld1q_u8(input);
    uint8x16_t round_key = vld1q_u8(key);
    
    // Use ARM64 crypto extensions if available
    #ifdef __ARM_FEATURE_CRYPTO
    data = vaeseq_u8(data, round_key);
    data = vaesmcq_u8(data);
    #else
    // Fallback to software implementation
    // This would contain the actual AES implementation
    #endif
    
    vst1q_u8(output, data);
}

// ARM64-specific branch prediction hints
#define ARM64_LIKELY(x) __builtin_expect(!!(x), 1)
#define ARM64_UNLIKELY(x) __builtin_expect(!!(x), 0)

// ARM64-specific memory barriers
#define ARM64_MEMORY_BARRIER() __asm__ __volatile__("dmb sy" ::: "memory")
#define ARM64_READ_BARRIER() __asm__ __volatile__("dmb ld" ::: "memory")
#define ARM64_WRITE_BARRIER() __asm__ __volatile__("dmb st" ::: "memory")

// ARM64-optimized bit manipulation
static inline uint64_t arm64_rotl64(uint64_t x, int n) {
    return __rorll(x, 64 - n);
}

static inline uint64_t arm64_rotr64(uint64_t x, int n) {
    return __rorll(x, n);
}

// ARM64-specific CRC32 acceleration
#ifdef __ARM_FEATURE_CRC32
static inline uint32_t arm64_crc32_u8(uint32_t crc, uint8_t data) {
    return __crc32cb(crc, data);
}

static inline uint32_t arm64_crc32_u32(uint32_t crc, uint32_t data) {
    return __crc32cw(crc, data);
}
#endif

#else
// Non-ARM64 builds: provide empty stubs or fallback to standard implementations
// This ensures the header can be safely included on all platforms

// Empty stubs for non-ARM64 platforms
#define ARM64_PREFETCH_READ(addr) ((void)0)
#define ARM64_PREFETCH_WRITE(addr) ((void)0)
#define ARM64_CACHE_LINE_SIZE 64

// Fallback to standard implementations
static inline void arm64_memcpy_optimized(void* dest, const void* src, size_t n) {
    memcpy(dest, src, n);
}

static inline void arm64_hash_block_neon(const uint8_t* input, uint8_t* output, size_t blocks) {
    // Fallback to standard implementation
    for (size_t i = 0; i < blocks; i++) {
        // Standard hash implementation would go here
        memcpy(output + i * 16, input + i * 16, 16);
    }
}

static inline void arm64_aes_encrypt_neon(const uint8_t* input, uint8_t* output, const uint8_t* key) {
    // Fallback to standard AES implementation
    memcpy(output, input, 16);
}

#define ARM64_LIKELY(x) (x)
#define ARM64_UNLIKELY(x) (x)

#define ARM64_MEMORY_BARRIER() ((void)0)
#define ARM64_READ_BARRIER() ((void)0)
#define ARM64_WRITE_BARRIER() ((void)0)

static inline uint64_t arm64_rotl64(uint64_t x, int n) {
    return (x << n) | (x >> (64 - n));
}

static inline uint64_t arm64_rotr64(uint64_t x, int n) {
    return (x >> n) | (x << (64 - n));
}

#ifdef __ARM_FEATURE_CRC32
static inline uint32_t arm64_crc32_u8(uint32_t crc, uint8_t data) {
    return crc ^ data; // Simple fallback
}

static inline uint32_t arm64_crc32_u32(uint32_t crc, uint32_t data) {
    return crc ^ data; // Simple fallback
}
#endif

#endif // __aarch64__ && __ARM_NEON

#endif // ARM64_OPTIMIZATIONS_H