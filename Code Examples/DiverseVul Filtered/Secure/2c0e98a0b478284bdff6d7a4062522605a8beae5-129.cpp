static int mov_write_packet(AVFormatContext *s, AVPacket *pkt)
{
    if (!pkt) {
        mov_flush_fragment(s, 1);
        return 1;
    } else {
        int i;
        MOVMuxContext *mov = s->priv_data;
        MOVTrack *trk = &mov->tracks[pkt->stream_index];

        if (!pkt->size)
            return mov_write_single_packet(s, pkt); /* Passthrough. */

        /*
         * Subtitles require special handling.
         *
         * 1) For full complaince, every track must have a sample at
         * dts == 0, which is rarely true for subtitles. So, as soon
         * as we see any packet with dts > 0, write an empty subtitle
         * at dts == 0 for any subtitle track with no samples in it.
         *
         * 2) For each subtitle track, check if the current packet's
         * dts is past the duration of the last subtitle sample. If
         * so, we now need to write an end sample for that subtitle.
         *
         * This must be done conditionally to allow for subtitles that
         * immediately replace each other, in which case an end sample
         * is not needed, and is, in fact, actively harmful.
         *
         * 3) See mov_write_trailer for how the final end sample is
         * handled.
         */
        for (i = 0; i < mov->nb_streams; i++) {
            MOVTrack *trk = &mov->tracks[i];
            int ret;

            if (trk->par->codec_id == AV_CODEC_ID_MOV_TEXT &&
                trk->track_duration < pkt->dts &&
                (trk->entry == 0 || !trk->last_sample_is_subtitle_end)) {
                ret = mov_write_subtitle_end_packet(s, i, trk->track_duration);
                if (ret < 0) return ret;
                trk->last_sample_is_subtitle_end = 1;
            }
        }

        if (trk->mode == MODE_MOV && trk->par->codec_type == AVMEDIA_TYPE_VIDEO) {
            AVPacket *opkt = pkt;
            int reshuffle_ret, ret;
            if (trk->is_unaligned_qt_rgb) {
                int64_t bpc = trk->par->bits_per_coded_sample != 15 ? trk->par->bits_per_coded_sample : 16;
                int expected_stride = ((trk->par->width * bpc + 15) >> 4)*2;
                reshuffle_ret = ff_reshuffle_raw_rgb(s, &pkt, trk->par, expected_stride);
                if (reshuffle_ret < 0)
                    return reshuffle_ret;
            } else
                reshuffle_ret = 0;
            if (trk->par->format == AV_PIX_FMT_PAL8 && !trk->pal_done) {
                ret = ff_get_packet_palette(s, opkt, reshuffle_ret, trk->palette);
                if (ret < 0)
                    goto fail;
                if (ret)
                    trk->pal_done++;
            } else if (trk->par->codec_id == AV_CODEC_ID_RAWVIDEO &&
                       (trk->par->format == AV_PIX_FMT_GRAY8 ||
                       trk->par->format == AV_PIX_FMT_MONOBLACK)) {
                for (i = 0; i < pkt->size; i++)
                    pkt->data[i] = ~pkt->data[i];
            }
            if (reshuffle_ret) {
                ret = mov_write_single_packet(s, pkt);
fail:
                if (reshuffle_ret)
                    av_packet_free(&pkt);
                return ret;
            }
        }

        return mov_write_single_packet(s, pkt);
    }
}