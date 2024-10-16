static int mov_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    MOVContext *mov = s->priv_data;
    MOVStreamContext *sc;
    AVIndexEntry *sample;
    AVStream *st = NULL;
    int64_t current_index;
    int ret;
    mov->fc = s;
 retry:
    sample = mov_find_next_sample(s, &st);
    if (!sample || (mov->next_root_atom && sample->pos > mov->next_root_atom)) {
        if (!mov->next_root_atom)
            return AVERROR_EOF;
        if ((ret = mov_switch_root(s, mov->next_root_atom, -1)) < 0)
            return ret;
        goto retry;
    }
    sc = st->priv_data;
    /* must be done just before reading, to avoid infinite loop on sample */
    current_index = sc->current_index;
    mov_current_sample_inc(sc);

    if (mov->next_root_atom) {
        sample->pos = FFMIN(sample->pos, mov->next_root_atom);
        sample->size = FFMIN(sample->size, (mov->next_root_atom - sample->pos));
    }

    if (st->discard != AVDISCARD_ALL) {
        int64_t ret64 = avio_seek(sc->pb, sample->pos, SEEK_SET);
        if (ret64 != sample->pos) {
            av_log(mov->fc, AV_LOG_ERROR, "stream %d, offset 0x%"PRIx64": partial file\n",
                   sc->ffindex, sample->pos);
            if (should_retry(sc->pb, ret64)) {
                mov_current_sample_dec(sc);
            } else if (ret64 < 0) {
                return (int)ret64;
            }
            return AVERROR_INVALIDDATA;
        }

        if (st->discard == AVDISCARD_NONKEY && !(sample->flags & AVINDEX_KEYFRAME)) {
            av_log(mov->fc, AV_LOG_DEBUG, "Nonkey frame from stream %d discarded due to AVDISCARD_NONKEY\n", sc->ffindex);
            goto retry;
        }

        if (st->codecpar->codec_id == AV_CODEC_ID_EIA_608 && sample->size > 8)
            ret = get_eia608_packet(sc->pb, pkt, sample->size);
        else
            ret = av_get_packet(sc->pb, pkt, sample->size);
        if (ret < 0) {
            if (should_retry(sc->pb, ret)) {
                mov_current_sample_dec(sc);
            }
            return ret;
        }
#if CONFIG_DV_DEMUXER
        if (mov->dv_demux && sc->dv_audio_container) {
            ret = avpriv_dv_produce_packet(mov->dv_demux, pkt, pkt->data, pkt->size, pkt->pos);
            av_packet_unref(pkt);
            if (ret < 0)
                return ret;
            ret = avpriv_dv_get_packet(mov->dv_demux, pkt);
            if (ret < 0)
                return ret;
        }
#endif
        if (sc->has_palette) {
            uint8_t *pal;

            pal = av_packet_new_side_data(pkt, AV_PKT_DATA_PALETTE, AVPALETTE_SIZE);
            if (!pal) {
                av_log(mov->fc, AV_LOG_ERROR, "Cannot append palette to packet\n");
            } else {
                memcpy(pal, sc->palette, AVPALETTE_SIZE);
                sc->has_palette = 0;
            }
        }
        if (st->codecpar->codec_id == AV_CODEC_ID_MP3 && !ffstream(st)->need_parsing && pkt->size > 4) {
            if (ff_mpa_check_header(AV_RB32(pkt->data)) < 0)
                ffstream(st)->need_parsing = AVSTREAM_PARSE_FULL;
        }
    }

    pkt->stream_index = sc->ffindex;
    pkt->dts = sample->timestamp;
    if (sample->flags & AVINDEX_DISCARD_FRAME) {
        pkt->flags |= AV_PKT_FLAG_DISCARD;
    }
    if (sc->ctts_data && sc->ctts_index < sc->ctts_count) {
        pkt->pts = pkt->dts + sc->dts_shift + sc->ctts_data[sc->ctts_index].duration;
        /* update ctts context */
        sc->ctts_sample++;
        if (sc->ctts_index < sc->ctts_count &&
            sc->ctts_data[sc->ctts_index].count == sc->ctts_sample) {
            sc->ctts_index++;
            sc->ctts_sample = 0;
        }
    } else {
        int64_t next_dts = (sc->current_sample < ffstream(st)->nb_index_entries) ?
            ffstream(st)->index_entries[sc->current_sample].timestamp : st->duration;

        if (next_dts >= pkt->dts)
            pkt->duration = next_dts - pkt->dts;
        pkt->pts = pkt->dts;
    }
    if (st->discard == AVDISCARD_ALL)
        goto retry;
    if (sc->sdtp_data && sc->current_sample <= sc->sdtp_count) {
        uint8_t sample_flags = sc->sdtp_data[sc->current_sample - 1];
        uint8_t sample_is_depended_on = (sample_flags >> 2) & 0x3;
        pkt->flags |= sample_is_depended_on == MOV_SAMPLE_DEPENDENCY_NO ? AV_PKT_FLAG_DISPOSABLE : 0;
    }
    pkt->flags |= sample->flags & AVINDEX_KEYFRAME ? AV_PKT_FLAG_KEY : 0;
    pkt->pos = sample->pos;

    /* Multiple stsd handling. */
    if (sc->stsc_data) {
        if (sc->stsc_data[sc->stsc_index].id > 0 &&
            sc->stsc_data[sc->stsc_index].id - 1 < sc->stsd_count &&
            sc->stsc_data[sc->stsc_index].id - 1 != sc->last_stsd_index) {
            ret = mov_change_extradata(sc, pkt);
            if (ret < 0)
                return ret;
        }

        /* Update the stsc index for the next sample */
        sc->stsc_sample++;
        if (mov_stsc_index_valid(sc->stsc_index, sc->stsc_count) &&
            mov_get_stsc_samples(sc, sc->stsc_index) == sc->stsc_sample) {
            sc->stsc_index++;
            sc->stsc_sample = 0;
        }
    }

    if (mov->aax_mode)
        aax_filter(pkt->data, pkt->size, mov);

    ret = cenc_filter(mov, st, sc, pkt, current_index);
    if (ret < 0) {
        return ret;
    }

    return 0;
}