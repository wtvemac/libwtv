
#ifndef __LIBWTV_LIBC_H
#define __LIBWTV_LIBC_H

#include <stdbool.h>
#include "libc/o1heap.h"
#include "libc/string.h"
#include "libc/io.h"

#ifdef __cplusplus
extern "C" {
#endif

//typedef int FILE;
typedef intptr_t ssize_t;

#define calloc(nelem, size) malloc(heap, nelem * size)

//EMAC: refactor. This is temporary
#define core_init_temp heap_init2
void heap_init2();

void heap_init(void* const base, const size_t size);
bool heap_check();
O1HeapDiagnostics heap_diag(void* const base, const size_t size);
void* malloc(const size_t amount);
void free(void* const pointer);

void* memalign(size_t alignment, const size_t amount);
void freealign(void* const pointer);

void register_putchar_callback(void (*callback)(const char character));
void unregister_putchar_callback(void (*callback)(const char character));

//EMAC:
void set_serial_output(bool output_enabled);
void set_callback_output(bool output_enabled);

//EMAC:
int toupper(int c);
//EMAC:
int stricmp(const char *s1, const char *s2);
int stricmp2(const char *s1, const char *s2, size_t n);

#ifdef __cplusplus
}
#endif

#endif