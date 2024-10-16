static int viv_read_packet(AVFormatContext *s,
                           AVPacket *pkt)
{
    VividasDemuxContext *viv = s->priv_data;
    AVIOContext *pb;
    int64_t off;
    int ret;

    if (!viv->sb_pb)
        return AVERROR(EIO);
    if (avio_feof(viv->sb_pb))
        return AVERROR_EOF;

    if (viv->current_audio_subpacket < viv->n_audio_subpackets) {
        AVStream *astream;
        int size = viv->audio_subpackets[viv->current_audio_subpacket+1].start - viv->audio_subpackets[viv->current_audio_subpacket].start;

        pb = viv->sb_pb;
        ret = av_get_packet(pb, pkt, size);
        if (ret < 0)
            return ret;
        pkt->pos += viv->sb_offset + viv->sb_blocks[viv->current_sb].byte_offset;

        pkt->stream_index = 1;
        astream = s->streams[pkt->stream_index];

        pkt->pts = av_rescale_q(viv->audio_sample, av_make_q(1, astream->codecpar->sample_rate), astream->time_base);
        viv->audio_sample += viv->audio_subpackets[viv->current_audio_subpacket].pcm_bytes / 2 / astream->codecpar->channels;
        pkt->flags |= AV_PKT_FLAG_KEY;
        viv->current_audio_subpacket++;
        return 0;
    }

    if (viv->current_sb_entry >= viv->n_sb_entries) {
        if (viv->current_sb+1 >= viv->n_sb_blocks)
            return AVERROR(EIO);
        viv->current_sb++;

        load_sb_block(s, viv, 0);
        viv->current_sb_entry = 0;
    }

    pb = viv->sb_pb;
    if (!pb)
        return AVERROR(EIO);
    off = avio_tell(pb);

    if (viv->current_sb_entry >= viv->n_sb_entries)
        return AVERROR_INVALIDDATA;

    off += viv->sb_entries[viv->current_sb_entry].size;

    if (viv->sb_entries[viv->current_sb_entry].flag == 0) {
        uint64_t v_size = ffio_read_varlen(pb);

        if (!viv->num_audio)
            return AVERROR_INVALIDDATA;

        ffio_read_varlen(pb);
        if (v_size > INT_MAX || !v_size)
            return AVERROR_INVALIDDATA;
        ret = av_get_packet(pb, pkt, v_size);
        if (ret < 0)
            return ret;
        pkt->pos += viv->sb_offset + viv->sb_blocks[viv->current_sb].byte_offset;

        pkt->pts = viv->sb_blocks[viv->current_sb].packet_offset + viv->current_sb_entry;
        pkt->flags |= (pkt->data[0]&0x80)?0:AV_PKT_FLAG_KEY;
        pkt->stream_index = 0;

        for (int i = 0; i < MAX_AUDIO_SUBPACKETS - 1; i++) {
            int start, pcm_bytes;
            start = ffio_read_varlen(pb);
            pcm_bytes = ffio_read_varlen(pb);

            if (i > 0 && start == 0)
                break;

            viv->n_audio_subpackets = i + 1;
            viv->audio_subpackets[i].start = start;
            viv->audio_subpackets[i].pcm_bytes = pcm_bytes;
        }
        viv->audio_subpackets[viv->n_audio_subpackets].start = (int)(off - avio_tell(pb));
        viv->current_audio_subpacket = 0;

    } else {
        uint64_t v_size = ffio_read_varlen(pb);

        if (v_size > INT_MAX || !v_size)
            return AVERROR_INVALIDDATA;
        ret = av_get_packet(pb, pkt, v_size);
        if (ret < 0)
            return ret;
        pkt->pos += viv->sb_offset + viv->sb_blocks[viv->current_sb].byte_offset;
        pkt->pts = viv->sb_blocks[viv->current_sb].packet_offset + viv->current_sb_entry;
        pkt->flags |= (pkt->data[0] & 0x80) ? 0 : AV_PKT_FLAG_KEY;
        pkt->stream_index = 0;
    }

    viv->current_sb_entry++;

    return 0;
}