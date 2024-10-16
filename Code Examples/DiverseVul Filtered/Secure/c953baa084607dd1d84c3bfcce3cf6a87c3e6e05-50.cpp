static int mov_read_sample_encryption_info(MOVContext *c, AVIOContext *pb, MOVStreamContext *sc, AVEncryptionInfo **sample, int use_subsamples)
{
    int i, ret;
    unsigned int subsample_count;
    AVSubsampleEncryptionInfo *subsamples;

    if (!sc->cenc.default_encrypted_sample) {
        av_log(c->fc, AV_LOG_ERROR, "Missing schm or tenc\n");
        return AVERROR_INVALIDDATA;
    }

    *sample = av_encryption_info_clone(sc->cenc.default_encrypted_sample);
    if (!*sample)
        return AVERROR(ENOMEM);

    if (sc->cenc.per_sample_iv_size != 0) {
        if ((ret = ffio_read_size(pb, (*sample)->iv, sc->cenc.per_sample_iv_size)) < 0) {
            av_log(c->fc, AV_LOG_ERROR, "failed to read the initialization vector\n");
            av_encryption_info_free(*sample);
            *sample = NULL;
            return ret;
        }
    }

    if (use_subsamples) {
        subsample_count = avio_rb16(pb);
        av_free((*sample)->subsamples);
        (*sample)->subsamples = av_calloc(subsample_count, sizeof(*subsamples));
        if (!(*sample)->subsamples) {
            av_encryption_info_free(*sample);
            *sample = NULL;
            return AVERROR(ENOMEM);
        }

        for (i = 0; i < subsample_count && !pb->eof_reached; i++) {
            (*sample)->subsamples[i].bytes_of_clear_data = avio_rb16(pb);
            (*sample)->subsamples[i].bytes_of_protected_data = avio_rb32(pb);
        }

        if (pb->eof_reached) {
            av_log(c->fc, AV_LOG_ERROR, "hit EOF while reading sub-sample encryption info\n");
            av_encryption_info_free(*sample);
            *sample = NULL;
            return AVERROR_INVALIDDATA;
        }
        (*sample)->subsample_count = subsample_count;
    }

    return 0;
}