static av_cold int aac_decode_close(AVCodecContext *avctx)
{
    AACContext *ac = avctx->priv_data;
    int i, type;

    for (i = 0; i < MAX_ELEM_ID; i++) {
        for (type = 0; type < 4; type++) {
            if (ac->che[type][i])
                ff_aac_sbr_ctx_close(&ac->che[type][i]->sbr);
            av_freep(&ac->che[type][i]);
        }
    }

    ff_mdct_end(&ac->mdct);
    ff_mdct_end(&ac->mdct_small);
    ff_mdct_end(&ac->mdct_ld);
    ff_mdct_end(&ac->mdct_ltp);
    return 0;
}