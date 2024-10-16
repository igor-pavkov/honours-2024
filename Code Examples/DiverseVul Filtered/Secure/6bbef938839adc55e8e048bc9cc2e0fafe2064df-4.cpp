static inline void mpeg4_encode_blocks(MpegEncContext *s, int16_t block[6][64],
                                       int intra_dc[6], uint8_t **scan_table,
                                       PutBitContext *dc_pb,
                                       PutBitContext *ac_pb)
{
    int i;

    if (scan_table) {
        if (s->avctx->flags2 & AV_CODEC_FLAG2_NO_OUTPUT) {
            for (i = 0; i < 6; i++)
                skip_put_bits(&s->pb,
                              mpeg4_get_block_length(s, block[i], i,
                                                     intra_dc[i], scan_table[i]));
        } else {
            /* encode each block */
            for (i = 0; i < 6; i++)
                mpeg4_encode_block(s, block[i], i,
                                   intra_dc[i], scan_table[i], dc_pb, ac_pb);
        }
    } else {
        if (s->avctx->flags2 & AV_CODEC_FLAG2_NO_OUTPUT) {
            for (i = 0; i < 6; i++)
                skip_put_bits(&s->pb,
                              mpeg4_get_block_length(s, block[i], i, 0,
                                                     s->intra_scantable.permutated));
        } else {
            /* encode each block */
            for (i = 0; i < 6; i++)
                mpeg4_encode_block(s, block[i], i, 0,
                                   s->intra_scantable.permutated, dc_pb, ac_pb);
        }
    }
}