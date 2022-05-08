#ifndef dbg_h
#define dbg_h

#include <stdio.h>
#include <errno.h>
#include <string.h>

// Disable debug() calls if NDEBUG set
#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%s:%d: " M "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#endif

// Get clean errno
#define clean_errno() (errno == 0 ? "None" : strerror(errno))

// Log functions
#define log_err(M, ...) fprintf(stderr, "[ERROR] (%s:%s:%d; errno: %s) " M "\n", __FILE__, __func__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define log_warn(M, ...) fprintf(stderr, "[WARN] (%s:%s:%d; errno: %s) " M "\n", __FILE__, __func__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%s:%d) " M "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__)

// Manual verification functions
// Only run if not compiled for library using LIB
#ifndef LIB
#define check(A, M, ...) if(!(A)) {log_err(M, ##__VA_ARGS__); errno=0; goto error; }
#define sentinel(M, ...) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }
#else
#define check(A, M, ...) if(!(A)) {errno=0; goto error; }
#define sentinel(M, ...) {errno=0; goto error; }
#endif

// Convenience verification functions
#define check_mem(A) check((A), "Out of memory.")
#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; }

#endif
