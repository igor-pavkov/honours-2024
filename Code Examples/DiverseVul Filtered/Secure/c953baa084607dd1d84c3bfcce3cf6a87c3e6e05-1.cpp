static int mov_read_saio(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    uint64_t *auxiliary_offsets;
    MOVEncryptionIndex *encryption_index;
    MOVStreamContext *sc;
    int i, ret;
    unsigned int version, entry_count, aux_info_type, aux_info_param;
    unsigned int alloc_size = 0;

    ret = get_current_encryption_info(c, &encryption_index, &sc);
    if (ret != 1)
        return ret;

    if (encryption_index->nb_encrypted_samples) {
        // This can happen if we have both saio/saiz and senc atoms.
        av_log(c->fc, AV_LOG_DEBUG, "Ignoring duplicate encryption info in saio\n");
        return 0;
    }

    if (encryption_index->auxiliary_offsets_count) {
        av_log(c->fc, AV_LOG_ERROR, "Duplicate saio atom\n");
        return AVERROR_INVALIDDATA;
    }

    version = avio_r8(pb); /* version */
    if (avio_rb24(pb) & 0x01) {  /* flags */
        aux_info_type = avio_rb32(pb);
        aux_info_param = avio_rb32(pb);
        if (sc->cenc.default_encrypted_sample) {
            if (aux_info_type != sc->cenc.default_encrypted_sample->scheme) {
                av_log(c->fc, AV_LOG_DEBUG, "Ignoring saio box with non-zero aux_info_type\n");
                return 0;
            }
            if (aux_info_param != 0) {
                av_log(c->fc, AV_LOG_DEBUG, "Ignoring saio box with non-zero aux_info_type_parameter\n");
                return 0;
            }
        } else {
            // Didn't see 'schm' or 'tenc', so this isn't encrypted.
            if ((aux_info_type == MKBETAG('c','e','n','c') ||
                 aux_info_type == MKBETAG('c','e','n','s') ||
                 aux_info_type == MKBETAG('c','b','c','1') ||
                 aux_info_type == MKBETAG('c','b','c','s')) &&
                aux_info_param == 0) {
                av_log(c->fc, AV_LOG_ERROR, "Saw encrypted saio without schm/tenc\n");
                return AVERROR_INVALIDDATA;
            } else {
                return 0;
            }
        }
    } else if (!sc->cenc.default_encrypted_sample) {
        // Didn't see 'schm' or 'tenc', so this isn't encrypted.
        return 0;
    }

    entry_count = avio_rb32(pb);
    if (entry_count >= INT_MAX / sizeof(*auxiliary_offsets))
        return AVERROR(ENOMEM);

    for (i = 0; i < entry_count && !pb->eof_reached; i++) {
        unsigned int min_offsets = FFMIN(FFMAX(i + 1, 1024), entry_count);
        auxiliary_offsets = av_fast_realloc(
            encryption_index->auxiliary_offsets, &alloc_size,
            min_offsets * sizeof(*auxiliary_offsets));
        if (!auxiliary_offsets) {
            av_freep(&encryption_index->auxiliary_offsets);
            return AVERROR(ENOMEM);
        }
        encryption_index->auxiliary_offsets = auxiliary_offsets;

        if (version == 0) {
            encryption_index->auxiliary_offsets[i] = avio_rb32(pb);
        } else {
            encryption_index->auxiliary_offsets[i] = avio_rb64(pb);
        }
        if (c->frag_index.current >= 0) {
            encryption_index->auxiliary_offsets[i] += c->fragment.base_data_offset;
        }
    }

    if (pb->eof_reached) {
        av_log(c->fc, AV_LOG_ERROR, "Hit EOF while reading saio\n");
        av_freep(&encryption_index->auxiliary_offsets);
        return AVERROR_INVALIDDATA;
    }

    encryption_index->auxiliary_offsets_count = entry_count;

    if (encryption_index->auxiliary_info_sample_count) {
        return mov_parse_auxiliary_info(c, sc, pb, encryption_index);
    }

    return 0;
}