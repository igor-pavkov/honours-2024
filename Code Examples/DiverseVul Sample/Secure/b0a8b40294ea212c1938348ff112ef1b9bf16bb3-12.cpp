static av_cold int decode_init(AVCodecContext *avctx)
{
    EXRContext *s = avctx->priv_data;
    uint32_t i;
    union av_intfloat32 t;
    float one_gamma = 1.0f / s->gamma;
    avpriv_trc_function trc_func = NULL;

    s->avctx              = avctx;

    ff_exrdsp_init(&s->dsp);

#if HAVE_BIGENDIAN
    ff_bswapdsp_init(&s->bbdsp);
#endif

    trc_func = avpriv_get_trc_function_from_trc(s->apply_trc_type);
    if (trc_func) {
        for (i = 0; i < 65536; ++i) {
            t = exr_half2float(i);
            t.f = trc_func(t.f);
            s->gamma_table[i] = t;
        }
    } else {
        if (one_gamma > 0.9999f && one_gamma < 1.0001f) {
            for (i = 0; i < 65536; ++i) {
                s->gamma_table[i] = exr_half2float(i);
            }
        } else {
            for (i = 0; i < 65536; ++i) {
                t = exr_half2float(i);
                /* If negative value we reuse half value */
                if (t.f <= 0.0f) {
                    s->gamma_table[i] = t;
                } else {
                    t.f = powf(t.f, one_gamma);
                    s->gamma_table[i] = t;
                }
            }
        }
    }

    // allocate thread data, used for non EXR_RAW compression types
    s->thread_data = av_mallocz_array(avctx->thread_count, sizeof(EXRThreadData));
    if (!s->thread_data)
        return AVERROR_INVALIDDATA;

    return 0;
}