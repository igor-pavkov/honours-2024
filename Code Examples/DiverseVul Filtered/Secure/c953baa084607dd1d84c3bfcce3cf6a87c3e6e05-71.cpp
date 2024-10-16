static int mov_read_tenc(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    MOVStreamContext *sc;
    unsigned int version, pattern, is_protected, iv_size;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];
    sc = st->priv_data;

    if (sc->pseudo_stream_id != 0) {
        av_log(c->fc, AV_LOG_ERROR, "tenc atom are only supported in first sample descriptor\n");
        return AVERROR_PATCHWELCOME;
    }

    if (!sc->cenc.default_encrypted_sample) {
        sc->cenc.default_encrypted_sample = av_encryption_info_alloc(0, 16, 16);
        if (!sc->cenc.default_encrypted_sample) {
            return AVERROR(ENOMEM);
        }
    }

    if (atom.size < 20)
        return AVERROR_INVALIDDATA;

    version = avio_r8(pb); /* version */
    avio_rb24(pb); /* flags */

    avio_r8(pb); /* reserved */
    pattern = avio_r8(pb);

    if (version > 0) {
        sc->cenc.default_encrypted_sample->crypt_byte_block = pattern >> 4;
        sc->cenc.default_encrypted_sample->skip_byte_block = pattern & 0xf;
    }

    is_protected = avio_r8(pb);
    if (is_protected && !sc->cenc.encryption_index) {
        // The whole stream should be by-default encrypted.
        sc->cenc.encryption_index = av_mallocz(sizeof(MOVEncryptionIndex));
        if (!sc->cenc.encryption_index)
            return AVERROR(ENOMEM);
    }
    sc->cenc.per_sample_iv_size = avio_r8(pb);
    if (sc->cenc.per_sample_iv_size != 0 && sc->cenc.per_sample_iv_size != 8 &&
        sc->cenc.per_sample_iv_size != 16) {
        av_log(c->fc, AV_LOG_ERROR, "invalid per-sample IV size value\n");
        return AVERROR_INVALIDDATA;
    }
    if (avio_read(pb, sc->cenc.default_encrypted_sample->key_id, 16) != 16) {
        av_log(c->fc, AV_LOG_ERROR, "failed to read the default key ID\n");
        return AVERROR_INVALIDDATA;
    }

    if (is_protected && !sc->cenc.per_sample_iv_size) {
        iv_size = avio_r8(pb);
        if (iv_size != 8 && iv_size != 16) {
            av_log(c->fc, AV_LOG_ERROR, "invalid default_constant_IV_size in tenc atom\n");
            return AVERROR_INVALIDDATA;
        }

        if (avio_read(pb, sc->cenc.default_encrypted_sample->iv, iv_size) != iv_size) {
            av_log(c->fc, AV_LOG_ERROR, "failed to read the default IV\n");
            return AVERROR_INVALIDDATA;
        }
    }

    return 0;
}