static int cenc_decrypt(MOVContext *c, MOVStreamContext *sc, AVEncryptionInfo *sample, uint8_t *input, int size)
{
    if (sample->scheme == MKBETAG('c','e','n','c') && !sample->crypt_byte_block && !sample->skip_byte_block) {
        return cenc_scheme_decrypt(c, sc, sample, input, size);
    } else if (sample->scheme == MKBETAG('c','b','c','1') && !sample->crypt_byte_block && !sample->skip_byte_block) {
        return cbc1_scheme_decrypt(c, sc, sample, input, size);
    } else if (sample->scheme == MKBETAG('c','e','n','s')) {
        return cens_scheme_decrypt(c, sc, sample, input, size);
    } else if (sample->scheme == MKBETAG('c','b','c','s')) {
        return cbcs_scheme_decrypt(c, sc, sample, input, size);
    } else {
        av_log(c->fc, AV_LOG_ERROR, "invalid encryption scheme\n");
        return AVERROR_INVALIDDATA;
    }
}