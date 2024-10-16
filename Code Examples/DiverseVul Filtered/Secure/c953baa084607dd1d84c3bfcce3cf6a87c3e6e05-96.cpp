static int cenc_scheme_decrypt(MOVContext *c, MOVStreamContext *sc, AVEncryptionInfo *sample, uint8_t *input, int size)
{
    int i, ret;
    int bytes_of_protected_data;

    if (!sc->cenc.aes_ctr) {
        /* initialize the cipher */
        sc->cenc.aes_ctr = av_aes_ctr_alloc();
        if (!sc->cenc.aes_ctr) {
            return AVERROR(ENOMEM);
        }

        ret = av_aes_ctr_init(sc->cenc.aes_ctr, c->decryption_key);
        if (ret < 0) {
            return ret;
        }
    }

    av_aes_ctr_set_full_iv(sc->cenc.aes_ctr, sample->iv);

    if (!sample->subsample_count) {
        /* decrypt the whole packet */
        av_aes_ctr_crypt(sc->cenc.aes_ctr, input, input, size);
        return 0;
    }

    for (i = 0; i < sample->subsample_count; i++) {
        if (sample->subsamples[i].bytes_of_clear_data + sample->subsamples[i].bytes_of_protected_data > size) {
            av_log(c->fc, AV_LOG_ERROR, "subsample size exceeds the packet size left\n");
            return AVERROR_INVALIDDATA;
        }

        /* skip the clear bytes */
        input += sample->subsamples[i].bytes_of_clear_data;
        size -= sample->subsamples[i].bytes_of_clear_data;

        /* decrypt the encrypted bytes */

        bytes_of_protected_data = sample->subsamples[i].bytes_of_protected_data;
        av_aes_ctr_crypt(sc->cenc.aes_ctr, input, input, bytes_of_protected_data);

        input += bytes_of_protected_data;
        size -= bytes_of_protected_data;
    }

    if (size > 0) {
        av_log(c->fc, AV_LOG_ERROR, "leftover packet bytes after subsample processing\n");
        return AVERROR_INVALIDDATA;
    }

    return 0;
}