static int hls_read_header(AVFormatContext *s)
{
    URLContext *u = (s->flags & AVFMT_FLAG_CUSTOM_IO) ? NULL : s->pb->opaque;
    HLSContext *c = s->priv_data;
    int ret = 0, i, j, stream_offset = 0;

    c->interrupt_callback = &s->interrupt_callback;

    c->first_packet = 1;
    c->first_timestamp = AV_NOPTS_VALUE;
    c->cur_timestamp = AV_NOPTS_VALUE;

    // if the URL context is good, read important options we must broker later
    if (u && u->prot->priv_data_class) {
        // get the previous user agent & set back to null if string size is zero
        update_options(&c->user_agent, "user-agent", u->priv_data);

        // get the previous cookies & set back to null if string size is zero
        update_options(&c->cookies, "cookies", u->priv_data);

        // get the previous headers & set back to null if string size is zero
        update_options(&c->headers, "headers", u->priv_data);
    }

    if ((ret = parse_playlist(c, s->filename, NULL, s->pb)) < 0)
        goto fail;

    if ((ret = save_avio_options(s)) < 0)
        goto fail;

    /* Some HLS servers don't like being sent the range header */
    av_dict_set(&c->avio_opts, "seekable", "0", 0);

    if (c->n_variants == 0) {
        av_log(NULL, AV_LOG_WARNING, "Empty playlist\n");
        ret = AVERROR_EOF;
        goto fail;
    }
    /* If the playlist only contained playlists (Master Playlist),
     * parse each individual playlist. */
    if (c->n_playlists > 1 || c->playlists[0]->n_segments == 0) {
        for (i = 0; i < c->n_playlists; i++) {
            struct playlist *pls = c->playlists[i];
            if ((ret = parse_playlist(c, pls->url, pls, NULL)) < 0)
                goto fail;
        }
    }

    if (c->variants[0]->playlists[0]->n_segments == 0) {
        av_log(NULL, AV_LOG_WARNING, "Empty playlist\n");
        ret = AVERROR_EOF;
        goto fail;
    }

    /* If this isn't a live stream, calculate the total duration of the
     * stream. */
    if (c->variants[0]->playlists[0]->finished) {
        int64_t duration = 0;
        for (i = 0; i < c->variants[0]->playlists[0]->n_segments; i++)
            duration += c->variants[0]->playlists[0]->segments[i]->duration;
        s->duration = duration;
    }

    /* Associate renditions with variants */
    for (i = 0; i < c->n_variants; i++) {
        struct variant *var = c->variants[i];

        if (var->audio_group[0])
            add_renditions_to_variant(c, var, AVMEDIA_TYPE_AUDIO, var->audio_group);
        if (var->video_group[0])
            add_renditions_to_variant(c, var, AVMEDIA_TYPE_VIDEO, var->video_group);
        if (var->subtitles_group[0])
            add_renditions_to_variant(c, var, AVMEDIA_TYPE_SUBTITLE, var->subtitles_group);
    }

    /* Open the demuxer for each playlist */
    for (i = 0; i < c->n_playlists; i++) {
        struct playlist *pls = c->playlists[i];
        AVInputFormat *in_fmt = NULL;

        if (!(pls->ctx = avformat_alloc_context())) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }

        if (pls->n_segments == 0)
            continue;

        pls->index  = i;
        pls->needed = 1;
        pls->parent = s;
        pls->cur_seq_no = select_cur_seq_no(c, pls);

        pls->read_buffer = av_malloc(INITIAL_BUFFER_SIZE);
        if (!pls->read_buffer){
            ret = AVERROR(ENOMEM);
            avformat_free_context(pls->ctx);
            pls->ctx = NULL;
            goto fail;
        }
        ffio_init_context(&pls->pb, pls->read_buffer, INITIAL_BUFFER_SIZE, 0, pls,
                          read_data, NULL, NULL);
        pls->pb.seekable = 0;
        ret = av_probe_input_buffer(&pls->pb, &in_fmt, pls->segments[0]->url,
                                    NULL, 0, 0);
        if (ret < 0) {
            /* Free the ctx - it isn't initialized properly at this point,
             * so avformat_close_input shouldn't be called. If
             * avformat_open_input fails below, it frees and zeros the
             * context, so it doesn't need any special treatment like this. */
            av_log(s, AV_LOG_ERROR, "Error when loading first segment '%s'\n", pls->segments[0]->url);
            avformat_free_context(pls->ctx);
            pls->ctx = NULL;
            goto fail;
        }
        pls->ctx->pb       = &pls->pb;
        pls->stream_offset = stream_offset;

        if ((ret = ff_copy_whitelists(pls->ctx, s)) < 0)
            goto fail;

        ret = avformat_open_input(&pls->ctx, pls->segments[0]->url, in_fmt, NULL);
        if (ret < 0)
            goto fail;

        if (pls->id3_deferred_extra && pls->ctx->nb_streams == 1) {
            ff_id3v2_parse_apic(pls->ctx, &pls->id3_deferred_extra);
            avformat_queue_attached_pictures(pls->ctx);
            ff_id3v2_free_extra_meta(&pls->id3_deferred_extra);
            pls->id3_deferred_extra = NULL;
        }

        pls->ctx->ctx_flags &= ~AVFMTCTX_NOHEADER;
        ret = avformat_find_stream_info(pls->ctx, NULL);
        if (ret < 0)
            goto fail;

        if (pls->is_id3_timestamped == -1)
            av_log(s, AV_LOG_WARNING, "No expected HTTP requests have been made\n");

        /* Create new AVStreams for each stream in this playlist */
        for (j = 0; j < pls->ctx->nb_streams; j++) {
            AVStream *st = avformat_new_stream(s, NULL);
            AVStream *ist = pls->ctx->streams[j];
            if (!st) {
                ret = AVERROR(ENOMEM);
                goto fail;
            }
            st->id = i;

            avcodec_copy_context(st->codec, pls->ctx->streams[j]->codec);

            if (pls->is_id3_timestamped) /* custom timestamps via id3 */
                avpriv_set_pts_info(st, 33, 1, MPEG_TIME_BASE);
            else
                avpriv_set_pts_info(st, ist->pts_wrap_bits, ist->time_base.num, ist->time_base.den);
        }

        add_metadata_from_renditions(s, pls, AVMEDIA_TYPE_AUDIO);
        add_metadata_from_renditions(s, pls, AVMEDIA_TYPE_VIDEO);
        add_metadata_from_renditions(s, pls, AVMEDIA_TYPE_SUBTITLE);

        stream_offset += pls->ctx->nb_streams;
    }

    /* Create a program for each variant */
    for (i = 0; i < c->n_variants; i++) {
        struct variant *v = c->variants[i];
        AVProgram *program;

        program = av_new_program(s, i);
        if (!program)
            goto fail;
        av_dict_set_int(&program->metadata, "variant_bitrate", v->bandwidth, 0);

        for (j = 0; j < v->n_playlists; j++) {
            struct playlist *pls = v->playlists[j];
            int is_shared = playlist_in_multiple_variants(c, pls);
            int k;

            for (k = 0; k < pls->ctx->nb_streams; k++) {
                struct AVStream *st = s->streams[pls->stream_offset + k];

                ff_program_add_stream_index(s, i, pls->stream_offset + k);

                /* Set variant_bitrate for streams unique to this variant */
                if (!is_shared && v->bandwidth)
                    av_dict_set_int(&st->metadata, "variant_bitrate", v->bandwidth, 0);
            }
        }
    }

    return 0;
fail:
    free_playlist_list(c);
    free_variant_list(c);
    free_rendition_list(c);
    return ret;
}