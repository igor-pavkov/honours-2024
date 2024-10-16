static int mxf_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    KLVPacket klv;
    MXFContext *mxf = s->priv_data;
    int ret;

    while (1) {
        int64_t max_data_size;
        int64_t pos = avio_tell(s->pb);

        if (pos < mxf->current_klv_data.next_klv - mxf->current_klv_data.length || pos >= mxf->current_klv_data.next_klv) {
            mxf->current_klv_data = (KLVPacket){{0}};
            ret = klv_read_packet(&klv, s->pb);
            if (ret < 0)
                break;
            max_data_size = klv.length;
            pos = klv.next_klv - klv.length;
            PRINT_KEY(s, "read packet", klv.key);
            av_log(s, AV_LOG_TRACE, "size %"PRIu64" offset %#"PRIx64"\n", klv.length, klv.offset);
            if (IS_KLV_KEY(klv.key, mxf_encrypted_triplet_key)) {
                ret = mxf_decrypt_triplet(s, pkt, &klv);
                if (ret < 0) {
                    av_log(s, AV_LOG_ERROR, "invalid encoded triplet\n");
                    return ret;
                }
                return 0;
            }
        } else {
            klv = mxf->current_klv_data;
            max_data_size = klv.next_klv - pos;
        }
        if (IS_KLV_KEY(klv.key, mxf_essence_element_key) ||
            IS_KLV_KEY(klv.key, mxf_canopus_essence_element_key) ||
            IS_KLV_KEY(klv.key, mxf_avid_essence_element_key)) {
            int body_sid = find_body_sid_by_offset(mxf, klv.offset);
            int index = mxf_get_stream_index(s, &klv, body_sid);
            int64_t next_ofs;
            AVStream *st;
            MXFTrack *track;

            if (index < 0) {
                av_log(s, AV_LOG_ERROR,
                       "error getting stream index %"PRIu32"\n",
                       AV_RB32(klv.key + 12));
                goto skip;
            }

            st = s->streams[index];
            track = st->priv_data;

            if (s->streams[index]->discard == AVDISCARD_ALL)
                goto skip;

            next_ofs = mxf_set_current_edit_unit(mxf, st, pos, 1);

            if (track->wrapping != FrameWrapped) {
                int64_t size;

                if (next_ofs <= 0) {
                    // If we have no way to packetize the data, then return it in chunks...
                    if (klv.next_klv - klv.length == pos && max_data_size > MXF_MAX_CHUNK_SIZE) {
                        st->need_parsing = AVSTREAM_PARSE_FULL;
                        avpriv_request_sample(s, "Huge KLV without proper index in non-frame wrapped essence");
                    }
                    size = FFMIN(max_data_size, MXF_MAX_CHUNK_SIZE);
                } else {
                    if ((size = next_ofs - pos) <= 0) {
                        av_log(s, AV_LOG_ERROR, "bad size: %"PRId64"\n", size);
                        ret = AVERROR_INVALIDDATA;
                        goto skip;
                    }
                    // We must not overread, because the next edit unit might be in another KLV
                    if (size > max_data_size)
                        size = max_data_size;
                }

                mxf->current_klv_data = klv;
                klv.offset = pos;
                klv.length = size;
                klv.next_klv = klv.offset + klv.length;
            }

            /* check for 8 channels AES3 element */
            if (klv.key[12] == 0x06 && klv.key[13] == 0x01 && klv.key[14] == 0x10) {
                ret = mxf_get_d10_aes3_packet(s->pb, s->streams[index],
                                              pkt, klv.length);
                if (ret < 0) {
                    av_log(s, AV_LOG_ERROR, "error reading D-10 aes3 frame\n");
                    mxf->current_klv_data = (KLVPacket){{0}};
                    return ret;
                }
            } else {
                ret = av_get_packet(s->pb, pkt, klv.length);
                if (ret < 0) {
                    mxf->current_klv_data = (KLVPacket){{0}};
                    return ret;
                }
            }
            pkt->stream_index = index;
            pkt->pos = klv.offset;

            ret = mxf_set_pts(mxf, st, pkt);
            if (ret < 0) {
                mxf->current_klv_data = (KLVPacket){{0}};
                return ret;
            }

            /* seek for truncated packets */
            avio_seek(s->pb, klv.next_klv, SEEK_SET);

            return 0;
        } else {
        skip:
            avio_skip(s->pb, max_data_size);
            mxf->current_klv_data = (KLVPacket){{0}};
        }
    }
    return avio_feof(s->pb) ? AVERROR_EOF : ret;
}