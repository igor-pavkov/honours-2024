static int frame_configure_elements(AVCodecContext *avctx)
{
    AACContext *ac = avctx->priv_data;
    int type, id, ch, ret;

    /* set channel pointers to internal buffers by default */
    for (type = 0; type < 4; type++) {
        for (id = 0; id < MAX_ELEM_ID; id++) {
            ChannelElement *che = ac->che[type][id];
            if (che) {
                che->ch[0].ret = che->ch[0].ret_buf;
                che->ch[1].ret = che->ch[1].ret_buf;
            }
        }
    }

    /* get output buffer */
    av_frame_unref(ac->frame);
    if (!avctx->channels)
        return 1;

    ac->frame->nb_samples = 2048;
    if ((ret = ff_get_buffer(avctx, ac->frame, 0)) < 0)
        return ret;

    /* map output channel pointers to AVFrame data */
    for (ch = 0; ch < avctx->channels; ch++) {
        if (ac->output_element[ch])
            ac->output_element[ch]->ret = (float *)ac->frame->extended_data[ch];
    }

    return 0;
}