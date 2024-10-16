static int mov_parse_auxiliary_info(MOVContext *c, MOVStreamContext *sc, AVIOContext *pb, MOVEncryptionIndex *encryption_index)
{
    AVEncryptionInfo **sample, **encrypted_samples;
    int64_t prev_pos;
    size_t sample_count, sample_info_size, i;
    int ret = 0;
    unsigned int alloc_size = 0;

    if (encryption_index->nb_encrypted_samples)
        return 0;
    sample_count = encryption_index->auxiliary_info_sample_count;
    if (encryption_index->auxiliary_offsets_count != 1) {
        av_log(c->fc, AV_LOG_ERROR, "Multiple auxiliary info chunks are not supported\n");
        return AVERROR_PATCHWELCOME;
    }
    if (sample_count >= INT_MAX / sizeof(*encrypted_samples))
        return AVERROR(ENOMEM);

    prev_pos = avio_tell(pb);
    if (!(pb->seekable & AVIO_SEEKABLE_NORMAL) ||
        avio_seek(pb, encryption_index->auxiliary_offsets[0], SEEK_SET) != encryption_index->auxiliary_offsets[0]) {
        av_log(c->fc, AV_LOG_INFO, "Failed to seek for auxiliary info, will only parse senc atoms for encryption info\n");
        goto finish;
    }

    for (i = 0; i < sample_count && !pb->eof_reached; i++) {
        unsigned int min_samples = FFMIN(FFMAX(i + 1, 1024 * 1024), sample_count);
        encrypted_samples = av_fast_realloc(encryption_index->encrypted_samples, &alloc_size,
                                            min_samples * sizeof(*encrypted_samples));
        if (!encrypted_samples) {
            ret = AVERROR(ENOMEM);
            goto finish;
        }
        encryption_index->encrypted_samples = encrypted_samples;

        sample = &encryption_index->encrypted_samples[i];
        sample_info_size = encryption_index->auxiliary_info_default_size
                               ? encryption_index->auxiliary_info_default_size
                               : encryption_index->auxiliary_info_sizes[i];

        ret = mov_read_sample_encryption_info(c, pb, sc, sample, sample_info_size > sc->cenc.per_sample_iv_size);
        if (ret < 0)
            goto finish;
    }
    if (pb->eof_reached) {
        av_log(c->fc, AV_LOG_ERROR, "Hit EOF while reading auxiliary info\n");
        ret = AVERROR_INVALIDDATA;
    } else {
        encryption_index->nb_encrypted_samples = sample_count;
    }

finish:
    avio_seek(pb, prev_pos, SEEK_SET);
    if (ret < 0) {
        for (; i > 0; i--) {
            av_encryption_info_free(encryption_index->encrypted_samples[i - 1]);
        }
        av_freep(&encryption_index->encrypted_samples);
    }
    return ret;
}