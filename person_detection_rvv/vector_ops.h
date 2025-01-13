#ifndef VECTOR_OPS_H
#define VECTOR_OPS_H

#ifdef __cplusplus
extern "C" {
#endif

void *memcpy_rvv(void *__restrict destination, const void *__restrict source,
                 size_t n);

#ifdef __cplusplus
}
#endif

#endif /* VECTOR_OPS_H */