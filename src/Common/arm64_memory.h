// ARM64-specific memory optimizations for Fuego
// Copyright (c) 2024 Fuego Developers

#ifndef ARM64_MEMORY_H
#define ARM64_MEMORY_H

// Only include ARM64 optimizations when building for ARM64
#if defined(__aarch64__)

#include <stdint.h>
#include <string.h>

// ARM64 cache line size (typically 64 bytes)
#define ARM64_CACHE_LINE_SIZE 64

// ARM64-specific memory prefetching
static inline void arm64_prefetch_read(const void* addr) {
    __builtin_prefetch(addr, 0, 3); // Read, temporal locality
}

static inline void arm64_prefetch_write(const void* addr) {
    __builtin_prefetch(addr, 1, 3); // Write, temporal locality
}

// ARM64-optimized memory copy with cache awareness
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
    
    // Prefetch source data
    arm64_prefetch_read(src);
    arm64_prefetch_read((const char*)src + ARM64_CACHE_LINE_SIZE);
    
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    
    // Align destination to 16-byte boundary for better performance
    while (((uintptr_t)d & 15) && n > 0) {
        *d++ = *s++;
        n--;
    }
    
    // Use 64-byte copies for better cache utilization
    while (n >= 64) {
        // Prefetch next cache line
        arm64_prefetch_read(s + ARM64_CACHE_LINE_SIZE);
        
        // Copy 64 bytes
        memcpy(d, s, 64);
        d += 64;
        s += 64;
        n -= 64;
    }
    
    // Handle remaining bytes
    while (n > 0) {
        *d++ = *s++;
        n--;
    }
}

// ARM64-optimized memory set with cache awareness
static inline void arm64_memset_optimized(void* ptr, int value, size_t num) {
    if (num < 16) {
        // Use scalar operations for small sets
        uint8_t* p = (uint8_t*)ptr;
        for (size_t i = 0; i < num; i++) {
            p[i] = (uint8_t)value;
        }
        return;
    }
    
    uint8_t* p = (uint8_t*)ptr;
    
    // Align to 16-byte boundary
    while (((uintptr_t)p & 15) && num > 0) {
        *p++ = (uint8_t)value;
        num--;
    }
    
    // Use 64-byte sets for better cache utilization
    while (num >= 64) {
        memset(p, value, 64);
        p += 64;
        num -= 64;
    }
    
    // Handle remaining bytes
    while (num > 0) {
        *p++ = (uint8_t)value;
        num--;
    }
}

// ARM64-specific memory barriers
static inline void arm64_memory_barrier() {
    __asm__ __volatile__("dmb sy" ::: "memory");
}

static inline void arm64_read_barrier() {
    __asm__ __volatile__("dmb ld" ::: "memory");
}

static inline void arm64_write_barrier() {
    __asm__ __volatile__("dmb st" ::: "memory");
}

// ARM64-optimized memory comparison
static inline int arm64_memcmp_optimized(const void* s1, const void* s2, size_t n) {
    if (n < 16) {
        // Use scalar operations for small comparisons
        const uint8_t* p1 = (const uint8_t*)s1;
        const uint8_t* p2 = (const uint8_t*)s2;
        for (size_t i = 0; i < n; i++) {
            if (p1[i] != p2[i]) {
                return p1[i] - p2[i];
            }
        }
        return 0;
    }
    
    const uint8_t* p1 = (const uint8_t*)s1;
    const uint8_t* p2 = (const uint8_t*)s2;
    
    // Align to 16-byte boundary
    while (((uintptr_t)p1 & 15) && n > 0) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
        n--;
    }
    
    // Use 64-byte comparisons for better cache utilization
    while (n >= 64) {
        int result = memcmp(p1, p2, 64);
        if (result != 0) {
            return result;
        }
        p1 += 64;
        p2 += 64;
        n -= 64;
    }
    
    // Handle remaining bytes
    while (n > 0) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
        n--;
    }
    
    return 0;
}

// ARM64-specific branch prediction hints
#define ARM64_LIKELY(x) __builtin_expect(!!(x), 1)
#define ARM64_UNLIKELY(x) __builtin_expect(!!(x), 0)

// ARM64-specific cache control
static inline void arm64_cache_flush(void* addr, size_t size) {
    // Flush cache lines containing the specified memory range
    char* start = (char*)((uintptr_t)addr & ~(ARM64_CACHE_LINE_SIZE - 1));
    char* end = (char*)((uintptr_t)((char*)addr + size) & ~(ARM64_CACHE_LINE_SIZE - 1));
    
    for (char* p = start; p <= end; p += ARM64_CACHE_LINE_SIZE) {
        __asm__ __volatile__("dc cvac, %0" : : "r"(p) : "memory");
    }
}

#else
// Non-ARM64 builds: provide fallback implementations
// This ensures the header can be safely included on all platforms

#include <stdint.h>
#include <stddef.h>
#include <string.h>

// Fallback implementations for non-ARM64 platforms
#define ARM64_CACHE_LINE_SIZE 64

static inline void arm64_prefetch_read(const void* addr) {
    __builtin_prefetch(addr, 0, 3);
}

static inline void arm64_prefetch_write(const void* addr) {
    __builtin_prefetch(addr, 1, 3);
}

static inline void arm64_memcpy_optimized(void* dest, const void* src, size_t n) {
    memcpy(dest, src, n);
}

static inline void arm64_memset_optimized(void* ptr, int value, size_t num) {
    memset(ptr, value, num);
}

static inline void arm64_memory_barrier() {
    __asm__ __volatile__("" ::: "memory");
}

static inline void arm64_read_barrier() {
    __asm__ __volatile__("" ::: "memory");
}

static inline void arm64_write_barrier() {
    __asm__ __volatile__("" ::: "memory");
}

static inline int arm64_memcmp_optimized(const void* s1, const void* s2, size_t n) {
    return memcmp(s1, s2, n);
}

#define ARM64_LIKELY(x) (x)
#define ARM64_UNLIKELY(x) (x)

static inline void arm64_cache_flush(void* addr, size_t size) {
    // No-op for non-ARM64 platforms
    (void)addr;
    (void)size;
}

#endif // __aarch64__

#endif // ARM64_MEMORY_H