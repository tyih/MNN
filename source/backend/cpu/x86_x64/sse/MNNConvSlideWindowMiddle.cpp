//
//  MNNConvSlideWindowMiddle.cpp
//  MNN
//
//  Created by MNN on 2019/02/04.
//  Copyright © 2018, Alibaba Group Holding Limited
//

#include <immintrin.h>
#include <stdint.h>

void _SSE_MNNConvSlideWindowMiddle(float* dst, const float* src, const float* weight, size_t width, size_t src_w_setup,
                              size_t src_depth_quad, size_t src_depth_step, size_t fw, size_t fh, size_t dilateX_step,
                              size_t dilateY_step, float* alpha) {
    int dx, sz, fx, fy;
    for (dx = 0; dx < width; ++dx) {
        float* dst_x  = dst + dx * 4;
        auto dstValue = _mm_set1_ps(0.0f);

        const float* src_dx = src + src_w_setup * dx;
        for (sz = 0; sz < src_depth_quad; ++sz) {
            const float* src_z    = src_dx + sz * src_depth_step;
            const float* weight_z = weight + sz * fh * fw * 16;
            for (fy = 0; fy < fh; ++fy) {
                const float* src_y    = src_z + fy * dilateY_step;
                const float* weight_y = weight_z + fy * fw * 16;
                for (fx = 0; fx < fw; ++fx) {
                    const float* weight_x = weight_y + 16 * fx;
                    const float* src_x    = src_y + fx * dilateX_step;
                    auto w0               = _mm_loadu_ps(weight_x + 4 * 0);
                    auto w1               = _mm_loadu_ps(weight_x + 4 * 1);
                    auto w2               = _mm_loadu_ps(weight_x + 4 * 2);
                    auto w3               = _mm_loadu_ps(weight_x + 4 * 3);

                    auto s0       = _mm_set1_ps(src_x[0]);
                    auto s1       = _mm_set1_ps(src_x[1]);
                    auto s2       = _mm_set1_ps(src_x[2]);
                    auto s3       = _mm_set1_ps(src_x[3]);

                    auto sw0 = _mm_mul_ps(s0, w0);
                    dstValue = _mm_add_ps(dstValue, sw0);
                    auto sw1 = _mm_mul_ps(s1, w1);
                    dstValue = _mm_add_ps(dstValue, sw1);
                    auto sw2 = _mm_mul_ps(s2, w2);
                    dstValue = _mm_add_ps(dstValue, sw2);
                    auto sw3 = _mm_mul_ps(s3, w3);
                    dstValue = _mm_add_ps(dstValue, sw3);
                }
            }
        }
        _mm_store_ps(dst_x, dstValue);
    }
}
