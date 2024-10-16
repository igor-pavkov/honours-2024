static bool demux_mf_read_packet(struct demuxer *demuxer,
                                 struct demux_packet **pkt)
{
    mf_t *mf = demuxer->priv;
    if (mf->curr_frame >= mf->nr_of_files)
        return false;
    bool ok = false;

    struct stream *entry_stream = NULL;
    if (mf->streams)
        entry_stream = mf->streams[mf->curr_frame];
    struct stream *stream = entry_stream;
    if (!stream) {
        char *filename = mf->names[mf->curr_frame];
        if (filename) {
            stream = stream_create(filename, demuxer->stream_origin | STREAM_READ,
                                   demuxer->cancel, demuxer->global);
        }
    }

    if (stream) {
        stream_seek(stream, 0);
        bstr data = stream_read_complete(stream, NULL, MF_MAX_FILE_SIZE);
        if (data.len) {
            demux_packet_t *dp = new_demux_packet(data.len);
            if (dp) {
                memcpy(dp->buffer, data.start, data.len);
                dp->pts = mf->curr_frame / mf->sh->codec->fps;
                dp->keyframe = true;
                dp->stream = mf->sh->index;
                *pkt = dp;
                ok = true;
            }
        }
        talloc_free(data.start);
    }

    if (stream && stream != entry_stream)
        free_stream(stream);

    mf->curr_frame++;

    if (!ok)
        MP_ERR(demuxer, "error reading image file\n");

    return true;
}