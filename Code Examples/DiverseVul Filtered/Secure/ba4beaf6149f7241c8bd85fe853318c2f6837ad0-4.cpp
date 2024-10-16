static int init_frame_decoder(APEContext *ctx)
{
    int i, ret;
    if ((ret = init_entropy_decoder(ctx)) < 0)
        return ret;
    init_predictor_decoder(ctx);

    for (i = 0; i < APE_FILTER_LEVELS; i++) {
        if (!ape_filter_orders[ctx->fset][i])
            break;
        init_filter(ctx, ctx->filters[i], ctx->filterbuf[i],
                    ape_filter_orders[ctx->fset][i]);
    }
    return 0;
}