static int demux_open_mf(demuxer_t *demuxer, enum demux_check check)
{
    mf_t *mf;

    if (strncmp(demuxer->stream->url, "mf://", 5) == 0 &&
        demuxer->stream->info && strcmp(demuxer->stream->info->name, "mf") == 0)
    {
        mf = open_mf_pattern(demuxer, demuxer, demuxer->stream->url + 5);
    } else {
        mf = open_mf_single(demuxer, demuxer->log, demuxer->stream->url);
        int bog = 0;
        MP_TARRAY_APPEND(mf, mf->streams, bog, demuxer->stream);
    }

    if (!mf || mf->nr_of_files < 1)
        goto error;

    double mf_fps;
    char *mf_type;
    mp_read_option_raw(demuxer->global, "mf-fps", &m_option_type_double, &mf_fps);
    mp_read_option_raw(demuxer->global, "mf-type", &m_option_type_string, &mf_type);

    const char *codec = mp_map_mimetype_to_video_codec(demuxer->stream->mime_type);
    if (!codec || (mf_type && mf_type[0]))
        codec = probe_format(mf, mf_type, check);
    talloc_free(mf_type);
    if (!codec)
        goto error;

    mf->curr_frame = 0;

    // create a new video stream header
    struct sh_stream *sh = demux_alloc_sh_stream(STREAM_VIDEO);
    struct mp_codec_params *c = sh->codec;

    c->codec = codec;
    c->disp_w = 0;
    c->disp_h = 0;
    c->fps = mf_fps;
    c->reliable_fps = true;

    demux_add_sh_stream(demuxer, sh);

    mf->sh = sh;
    demuxer->priv = (void *)mf;
    demuxer->seekable = true;
    demuxer->duration = mf->nr_of_files / mf->sh->codec->fps;

    return 0;

error:
    return -1;
}