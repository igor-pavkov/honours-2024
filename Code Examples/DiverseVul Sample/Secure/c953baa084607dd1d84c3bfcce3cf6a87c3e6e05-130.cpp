static int mov_read_senc(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVEncryptionInfo **encrypted_samples;
    MOVEncryptionIndex *encryption_index;
    MOVStreamContext *sc;
    int use_subsamples, ret;
    unsigned int sample_count, i, alloc_size = 0;

    ret = get_current_encryption_info(c, &encryption_index, &sc);
    if (ret != 1)
        return ret;

    if (encryption_index->nb_encrypted_samples) {
        // This can happen if we have both saio/saiz and senc atoms.
        av_log(c->fc, AV_LOG_DEBUG, "Ignoring duplicate encryption info in senc\n");
        return 0;
    }

    avio_r8(pb); /* version */
    use_subsamples = avio_rb24(pb) & 0x02; /* flags */

    sample_count = avio_rb32(pb);
    if (sample_count >= INT_MAX / sizeof(*encrypted_samples))
        return AVERROR(ENOMEM);

    for (i = 0; i < sample_count; i++) {
        unsigned int min_samples = FFMIN(FFMAX(i + 1, 1024 * 1024), sample_count);
        encrypted_samples = av_fast_realloc(encryption_index->encrypted_samples, &alloc_size,
                                            min_samples * sizeof(*encrypted_samples));
        if (encrypted_samples) {
            encryption_index->encrypted_samples = encrypted_samples;

            ret = mov_read_sample_encryption_info(
                c, pb, sc, &encryption_index->encrypted_samples[i], use_subsamples);
        } else {
            ret = AVERROR(ENOMEM);
        }
        if (pb->eof_reached) {
            av_log(c->fc, AV_LOG_ERROR, "Hit EOF while reading senc\n");
            if (ret >= 0)
                av_encryption_info_free(encryption_index->encrypted_samples[i]);
            ret = AVERROR_INVALIDDATA;
        }

        if (ret < 0) {
            for (; i > 0; i--)
                av_encryption_info_free(encryption_index->encrypted_samples[i - 1]);
            av_freep(&encryption_index->encrypted_samples);
            return ret;
        }
    }
    encryption_index->nb_encrypted_samples = sample_count;

    return 0;
}