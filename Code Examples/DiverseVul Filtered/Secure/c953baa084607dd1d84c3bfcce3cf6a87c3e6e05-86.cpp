static int mov_aaxc_crypto(MOVContext *c)
{
    if (c->audible_key_size != 16) {
        av_log(c->fc, AV_LOG_FATAL, "[aaxc] audible_key value needs to be 16 bytes!\n");
        return AVERROR(EINVAL);
    }

    if (c->audible_iv_size != 16) {
        av_log(c->fc, AV_LOG_FATAL, "[aaxc] audible_iv value needs to be 16 bytes!\n");
        return AVERROR(EINVAL);
    }

    c->aes_decrypt = av_aes_alloc();
    if (!c->aes_decrypt) {
        return AVERROR(ENOMEM);
    }

    memcpy(c->file_key, c->audible_key, 16);
    memcpy(c->file_iv, c->audible_iv, 16);
    c->aax_mode = 1;

    return 0;
}