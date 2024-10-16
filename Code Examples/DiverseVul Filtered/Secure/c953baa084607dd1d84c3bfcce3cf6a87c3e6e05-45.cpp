static int mov_read_saiz(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    MOVEncryptionIndex *encryption_index;
    MOVStreamContext *sc;
    int ret;
    unsigned int sample_count, aux_info_type, aux_info_param;

    ret = get_current_encryption_info(c, &encryption_index, &sc);
    if (ret != 1)
        return ret;

    if (encryption_index->nb_encrypted_samples) {
        // This can happen if we have both saio/saiz and senc atoms.
        av_log(c->fc, AV_LOG_DEBUG, "Ignoring duplicate encryption info in saiz\n");
        return 0;
    }

    if (encryption_index->auxiliary_info_sample_count) {
        av_log(c->fc, AV_LOG_ERROR, "Duplicate saiz atom\n");
        return AVERROR_INVALIDDATA;
    }

    avio_r8(pb); /* version */
    if (avio_rb24(pb) & 0x01) {  /* flags */
        aux_info_type = avio_rb32(pb);
        aux_info_param = avio_rb32(pb);
        if (sc->cenc.default_encrypted_sample) {
            if (aux_info_type != sc->cenc.default_encrypted_sample->scheme) {
                av_log(c->fc, AV_LOG_DEBUG, "Ignoring saiz box with non-zero aux_info_type\n");
                return 0;
            }
            if (aux_info_param != 0) {
                av_log(c->fc, AV_LOG_DEBUG, "Ignoring saiz box with non-zero aux_info_type_parameter\n");
                return 0;
            }
        } else {
            // Didn't see 'schm' or 'tenc', so this isn't encrypted.
            if ((aux_info_type == MKBETAG('c','e','n','c') ||
                 aux_info_type == MKBETAG('c','e','n','s') ||
                 aux_info_type == MKBETAG('c','b','c','1') ||
                 aux_info_type == MKBETAG('c','b','c','s')) &&
                aux_info_param == 0) {
                av_log(c->fc, AV_LOG_ERROR, "Saw encrypted saiz without schm/tenc\n");
                return AVERROR_INVALIDDATA;
            } else {
                return 0;
            }
        }
    } else if (!sc->cenc.default_encrypted_sample) {
        // Didn't see 'schm' or 'tenc', so this isn't encrypted.
        return 0;
    }

    encryption_index->auxiliary_info_default_size = avio_r8(pb);
    sample_count = avio_rb32(pb);
    encryption_index->auxiliary_info_sample_count = sample_count;

    if (encryption_index->auxiliary_info_default_size == 0) {
        ret = mov_try_read_block(pb, sample_count, &encryption_index->auxiliary_info_sizes);
        if (ret < 0) {
            av_log(c->fc, AV_LOG_ERROR, "Failed to read the auxiliary info\n");
            return ret;
        }
    }

    if (encryption_index->auxiliary_offsets_count) {
        return mov_parse_auxiliary_info(c, sc, pb, encryption_index);
    }

    return 0;
}