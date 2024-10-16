static int cenc_filter(MOVContext *mov, AVStream* st, MOVStreamContext *sc, AVPacket *pkt, int current_index)
{
    MOVFragmentStreamInfo *frag_stream_info;
    MOVEncryptionIndex *encryption_index;
    AVEncryptionInfo *encrypted_sample;
    int encrypted_index, ret;

    frag_stream_info = get_frag_stream_info_from_pkt(&mov->frag_index, pkt, st->id);
    encrypted_index = current_index;
    encryption_index = NULL;
    if (frag_stream_info) {
        // Note this only supports encryption info in the first sample descriptor.
        if (mov->fragment.stsd_id == 1) {
            if (frag_stream_info->encryption_index) {
                encrypted_index = current_index - frag_stream_info->index_base;
                encryption_index = frag_stream_info->encryption_index;
            } else {
                encryption_index = sc->cenc.encryption_index;
            }
        }
    } else {
        encryption_index = sc->cenc.encryption_index;
    }

    if (encryption_index) {
        if (encryption_index->auxiliary_info_sample_count &&
            !encryption_index->nb_encrypted_samples) {
            av_log(mov->fc, AV_LOG_ERROR, "saiz atom found without saio\n");
            return AVERROR_INVALIDDATA;
        }
        if (encryption_index->auxiliary_offsets_count &&
            !encryption_index->nb_encrypted_samples) {
            av_log(mov->fc, AV_LOG_ERROR, "saio atom found without saiz\n");
            return AVERROR_INVALIDDATA;
        }

        if (!encryption_index->nb_encrypted_samples) {
            // Full-sample encryption with default settings.
            encrypted_sample = sc->cenc.default_encrypted_sample;
        } else if (encrypted_index >= 0 && encrypted_index < encryption_index->nb_encrypted_samples) {
            // Per-sample setting override.
            encrypted_sample = encryption_index->encrypted_samples[encrypted_index];
        } else {
            av_log(mov->fc, AV_LOG_ERROR, "Incorrect number of samples in encryption info\n");
            return AVERROR_INVALIDDATA;
        }

        if (mov->decryption_key) {
            return cenc_decrypt(mov, sc, encrypted_sample, pkt->data, pkt->size);
        } else {
            size_t size;
            uint8_t *side_data = av_encryption_info_add_side_data(encrypted_sample, &size);
            if (!side_data)
                return AVERROR(ENOMEM);
            ret = av_packet_add_side_data(pkt, AV_PKT_DATA_ENCRYPTION_INFO, side_data, size);
            if (ret < 0)
                av_free(side_data);
            return ret;
        }
    }

    return 0;
}