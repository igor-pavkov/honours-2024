static int cbc1_scheme_decrypt(MOVContext *c, MOVStreamContext *sc, AVEncryptionInfo *sample, uint8_t *input, int size)
{
    int i, ret;
    int num_of_encrypted_blocks;
    uint8_t iv[16];

    if (!sc->cenc.aes_ctx) {
        /* initialize the cipher */
        sc->cenc.aes_ctx = av_aes_alloc();
        if (!sc->cenc.aes_ctx) {
            return AVERROR(ENOMEM);
        }

        ret = av_aes_init(sc->cenc.aes_ctx, c->decryption_key, 16 * 8, 1);
        if (ret < 0) {
            return ret;
        }
    }

    memcpy(iv, sample->iv, 16);

    /* whole-block full sample encryption */
    if (!sample->subsample_count) {
        /* decrypt the whole packet */
        av_aes_crypt(sc->cenc.aes_ctx, input, input, size/16, iv, 1);
        return 0;
    }

    for (i = 0; i < sample->subsample_count; i++) {
        if (sample->subsamples[i].bytes_of_clear_data + sample->subsamples[i].bytes_of_protected_data > size) {
            av_log(c->fc, AV_LOG_ERROR, "subsample size exceeds the packet size left\n");
            return AVERROR_INVALIDDATA;
        }

        if (sample->subsamples[i].bytes_of_protected_data % 16) {
            av_log(c->fc, AV_LOG_ERROR, "subsample BytesOfProtectedData is not a multiple of 16\n");
            return AVERROR_INVALIDDATA;
        }

        /* skip the clear bytes */
        input += sample->subsamples[i].bytes_of_clear_data;
        size -= sample->subsamples[i].bytes_of_clear_data;

        /* decrypt the encrypted bytes */
        num_of_encrypted_blocks = sample->subsamples[i].bytes_of_protected_data/16;
        if (num_of_encrypted_blocks > 0) {
            av_aes_crypt(sc->cenc.aes_ctx, input, input, num_of_encrypted_blocks, iv, 1);
        }
        input += sample->subsamples[i].bytes_of_protected_data;
        size -= sample->subsamples[i].bytes_of_protected_data;
    }

    if (size > 0) {
        av_log(c->fc, AV_LOG_ERROR, "leftover packet bytes after subsample processing\n");
        return AVERROR_INVALIDDATA;
    }

    return 0;
}