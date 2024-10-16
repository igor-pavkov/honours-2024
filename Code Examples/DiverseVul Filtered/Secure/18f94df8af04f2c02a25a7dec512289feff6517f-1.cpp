static int check_specific_config(ALSDecContext *ctx)
{
    ALSSpecificConfig *sconf = &ctx->sconf;
    int error = 0;

    // report unsupported feature and set error value
    #define MISSING_ERR(cond, str, errval)              \
    {                                                   \
        if (cond) {                                     \
            avpriv_report_missing_feature(ctx->avctx,   \
                                          str);         \
            error = errval;                             \
        }                                               \
    }

    MISSING_ERR(sconf->floating,  "Floating point decoding",     AVERROR_PATCHWELCOME);
    MISSING_ERR(sconf->rlslms,    "Adaptive RLS-LMS prediction", AVERROR_PATCHWELCOME);

    return error;
}