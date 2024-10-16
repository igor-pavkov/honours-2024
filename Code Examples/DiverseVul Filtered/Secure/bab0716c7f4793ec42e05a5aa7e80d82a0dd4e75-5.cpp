static int mxf_read_header(AVFormatContext *s)
{
    MXFContext *mxf = s->priv_data;
    KLVPacket klv;
    int64_t essence_offset = 0;
    int ret;

    mxf->last_forward_tell = INT64_MAX;

    if (!mxf_read_sync(s->pb, mxf_header_partition_pack_key, 14)) {
        av_log(s, AV_LOG_ERROR, "could not find header partition pack key\n");
        return AVERROR_INVALIDDATA;
    }
    avio_seek(s->pb, -14, SEEK_CUR);
    mxf->fc = s;
    mxf->run_in = avio_tell(s->pb);

    mxf_read_random_index_pack(s);

    while (!avio_feof(s->pb)) {
        const MXFMetadataReadTableEntry *metadata;

        if (klv_read_packet(&klv, s->pb) < 0) {
            /* EOF - seek to previous partition or stop */
            if(mxf_parse_handle_partition_or_eof(mxf) <= 0)
                break;
            else
                continue;
        }

        PRINT_KEY(s, "read header", klv.key);
        av_log(s, AV_LOG_TRACE, "size %"PRIu64" offset %#"PRIx64"\n", klv.length, klv.offset);
        if (IS_KLV_KEY(klv.key, mxf_encrypted_triplet_key) ||
            IS_KLV_KEY(klv.key, mxf_essence_element_key) ||
            IS_KLV_KEY(klv.key, mxf_canopus_essence_element_key) ||
            IS_KLV_KEY(klv.key, mxf_avid_essence_element_key) ||
            IS_KLV_KEY(klv.key, mxf_system_item_key_cp) ||
            IS_KLV_KEY(klv.key, mxf_system_item_key_gc)) {

            if (!mxf->current_partition) {
                av_log(mxf->fc, AV_LOG_ERROR, "found essence prior to first PartitionPack\n");
                return AVERROR_INVALIDDATA;
            }

            if (!mxf->current_partition->first_essence_klv.offset)
                mxf->current_partition->first_essence_klv = klv;

            if (!essence_offset)
                essence_offset = klv.offset;

            /* seek to footer, previous partition or stop */
            if (mxf_parse_handle_essence(mxf) <= 0)
                break;
            continue;
        } else if (mxf_is_partition_pack_key(klv.key) && mxf->current_partition) {
            /* next partition pack - keep going, seek to previous partition or stop */
            if(mxf_parse_handle_partition_or_eof(mxf) <= 0)
                break;
            else if (mxf->parsing_backward)
                continue;
            /* we're still parsing forward. proceed to parsing this partition pack */
        }

        for (metadata = mxf_metadata_read_table; metadata->read; metadata++) {
            if (IS_KLV_KEY(klv.key, metadata->key)) {
                if ((ret = mxf_parse_klv(mxf, klv, metadata->read, metadata->ctx_size, metadata->type)) < 0)
                    goto fail;
                break;
            }
        }
        if (!metadata->read) {
            av_log(s, AV_LOG_VERBOSE, "Dark key " PRIxUID "\n",
                            UID_ARG(klv.key));
            avio_skip(s->pb, klv.length);
        }
    }
    /* FIXME avoid seek */
    if (!essence_offset)  {
        av_log(s, AV_LOG_ERROR, "no essence\n");
        ret = AVERROR_INVALIDDATA;
        goto fail;
    }
    avio_seek(s->pb, essence_offset, SEEK_SET);

    /* we need to do this before computing the index tables
     * to be able to fill in zero IndexDurations with st->duration */
    if ((ret = mxf_parse_structural_metadata(mxf)) < 0)
        goto fail;

    for (int i = 0; i < s->nb_streams; i++)
        mxf_handle_missing_index_segment(mxf, s->streams[i]);

    if ((ret = mxf_compute_index_tables(mxf)) < 0)
        goto fail;

    if (mxf->nb_index_tables > 1) {
        /* TODO: look up which IndexSID to use via EssenceContainerData */
        av_log(mxf->fc, AV_LOG_INFO, "got %i index tables - only the first one (IndexSID %i) will be used\n",
               mxf->nb_index_tables, mxf->index_tables[0].index_sid);
    } else if (mxf->nb_index_tables == 0 && mxf->op == OPAtom && (s->error_recognition & AV_EF_EXPLODE)) {
        av_log(mxf->fc, AV_LOG_ERROR, "cannot demux OPAtom without an index\n");
        ret = AVERROR_INVALIDDATA;
        goto fail;
    }

    mxf_compute_essence_containers(s);

    for (int i = 0; i < s->nb_streams; i++)
        mxf_compute_edit_units_per_packet(mxf, s->streams[i]);

    return 0;
fail:
    mxf_read_close(s);

    return ret;
}