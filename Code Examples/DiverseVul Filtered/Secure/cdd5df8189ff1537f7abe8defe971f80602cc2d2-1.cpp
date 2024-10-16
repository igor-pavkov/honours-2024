static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    FPSContext        *s = ctx->priv;
    int frames_out = s->frames_out;
    int ret = 0;

    while (ret >= 0 && s->frames_out == frames_out)
        ret = ff_request_frame(ctx->inputs[0]);

    /* flush the fifo */
    if (ret == AVERROR_EOF && av_fifo_size(s->fifo)) {
        int i;
        for (i = 0; av_fifo_size(s->fifo); i++) {
            AVFrame *buf;

            av_fifo_generic_read(s->fifo, &buf, sizeof(buf), NULL);
            buf->pts = av_rescale_q(s->first_pts, ctx->inputs[0]->time_base,
                                    outlink->time_base) + s->frames_out;

            if ((ret = ff_filter_frame(outlink, buf)) < 0)
                return ret;

            s->frames_out++;
        }
        return 0;
    }

    return ret;
}