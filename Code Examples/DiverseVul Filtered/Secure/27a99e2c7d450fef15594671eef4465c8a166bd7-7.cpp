static int viv_read_seek(AVFormatContext *s, int stream_index, int64_t timestamp, int flags)
{
    VividasDemuxContext *viv = s->priv_data;
    int64_t frame;

    if (stream_index == 0)
        frame = timestamp;
    else
        frame = av_rescale_q(timestamp, s->streams[0]->time_base, s->streams[stream_index]->time_base);

    for (int i = 0; i < viv->n_sb_blocks; i++) {
        if (frame >= viv->sb_blocks[i].packet_offset && frame < viv->sb_blocks[i].packet_offset + viv->sb_blocks[i].n_packets) {
            // flush audio packet queue
            viv->current_audio_subpacket = 0;
            viv->n_audio_subpackets = 0;
            viv->current_sb = i;
            // seek to ith sb block
            avio_seek(s->pb, viv->sb_offset + viv->sb_blocks[i].byte_offset, SEEK_SET);
            // load the block
            load_sb_block(s, viv, 0);
            // most problematic part: guess audio offset
            viv->audio_sample = av_rescale_q(viv->sb_blocks[i].packet_offset, av_make_q(s->streams[1]->codecpar->sample_rate, 1), av_inv_q(s->streams[0]->time_base));
            // hand-tuned 1.s a/v offset
            viv->audio_sample += s->streams[1]->codecpar->sample_rate;
            viv->current_sb_entry = 0;
            return 1;
        }
    }
    return 0;
}