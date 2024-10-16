static void flush(AVCodecContext *avctx)
{
    AACContext *ac= avctx->priv_data;
    int type, i, j;

    for (type = 3; type >= 0; type--) {
        for (i = 0; i < MAX_ELEM_ID; i++) {
            ChannelElement *che = ac->che[type][i];
            if (che) {
                for (j = 0; j <= 1; j++) {
                    memset(che->ch[j].saved, 0, sizeof(che->ch[j].saved));
                }
            }
        }
    }
}