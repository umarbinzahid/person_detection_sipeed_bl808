#include <riscv_vector.h>
#include <stdio.h>
#include "vector_ops.h"

void *memcpy_rvv(void *__restrict destination, const void *__restrict source,
                 size_t n)
{
    unsigned char *dst = destination;
    const unsigned char *src = source;
    // copy data byte by byte
    for (size_t vl; n > 0; n -= vl, src += vl, dst += vl)
    {
        vl = vsetvl_e8m8(n);
        vuint8m8_t vec_src = vle8_v_u8m8(src, vl);
        vse8_v_u8m8(dst, vec_src, vl);
    }
    return destination;
}