static void mov_estimate_video_delay(MOVContext *c, AVStream* st)
{
    MOVStreamContext *msc = st->priv_data;
    FFStream *const sti = ffstream(st);
    int ctts_ind = 0;
    int ctts_sample = 0;
    int64_t pts_buf[MAX_REORDER_DELAY + 1]; // Circular buffer to sort pts.
    int buf_start = 0;
    int j, r, num_swaps;

    for (j = 0; j < MAX_REORDER_DELAY + 1; j++)
        pts_buf[j] = INT64_MIN;

    if (st->codecpar->video_delay <= 0 && msc->ctts_data &&
        st->codecpar->codec_id == AV_CODEC_ID_H264) {
        st->codecpar->video_delay = 0;
        for (int ind = 0; ind < sti->nb_index_entries && ctts_ind < msc->ctts_count; ++ind) {
            // Point j to the last elem of the buffer and insert the current pts there.
            j = buf_start;
            buf_start = (buf_start + 1);
            if (buf_start == MAX_REORDER_DELAY + 1)
                buf_start = 0;

            pts_buf[j] = sti->index_entries[ind].timestamp + msc->ctts_data[ctts_ind].duration;

            // The timestamps that are already in the sorted buffer, and are greater than the
            // current pts, are exactly the timestamps that need to be buffered to output PTS
            // in correct sorted order.
            // Hence the video delay (which is the buffer size used to sort DTS and output PTS),
            // can be computed as the maximum no. of swaps any particular timestamp needs to
            // go through, to keep this buffer in sorted order.
            num_swaps = 0;
            while (j != buf_start) {
                r = j - 1;
                if (r < 0) r = MAX_REORDER_DELAY;
                if (pts_buf[j] < pts_buf[r]) {
                    FFSWAP(int64_t, pts_buf[j], pts_buf[r]);
                    ++num_swaps;
                } else {
                    break;
                }
                j = r;
            }
            st->codecpar->video_delay = FFMAX(st->codecpar->video_delay, num_swaps);

            ctts_sample++;
            if (ctts_sample == msc->ctts_data[ctts_ind].count) {
                ctts_ind++;
                ctts_sample = 0;
            }
        }
        av_log(c->fc, AV_LOG_DEBUG, "Setting codecpar->delay to %d for stream st: %d\n",
               st->codecpar->video_delay, st->index);
    }
}