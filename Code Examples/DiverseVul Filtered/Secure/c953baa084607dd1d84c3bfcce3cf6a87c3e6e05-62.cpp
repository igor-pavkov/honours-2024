static int mov_read_close(AVFormatContext *s)
{
    MOVContext *mov = s->priv_data;
    int i, j;

    for (i = 0; i < s->nb_streams; i++) {
        AVStream *st = s->streams[i];
        MOVStreamContext *sc = st->priv_data;

        if (!sc)
            continue;

        av_freep(&sc->ctts_data);
        for (j = 0; j < sc->drefs_count; j++) {
            av_freep(&sc->drefs[j].path);
            av_freep(&sc->drefs[j].dir);
        }
        av_freep(&sc->drefs);

        sc->drefs_count = 0;

        if (!sc->pb_is_copied)
            ff_format_io_close(s, &sc->pb);

        sc->pb = NULL;
        av_freep(&sc->chunk_offsets);
        av_freep(&sc->stsc_data);
        av_freep(&sc->sample_sizes);
        av_freep(&sc->keyframes);
        av_freep(&sc->stts_data);
        av_freep(&sc->sdtp_data);
        av_freep(&sc->stps_data);
        av_freep(&sc->elst_data);
        av_freep(&sc->rap_group);
        av_freep(&sc->sync_group);
        av_freep(&sc->sgpd_sync);
        av_freep(&sc->sample_offsets);
        av_freep(&sc->open_key_samples);
        av_freep(&sc->display_matrix);
        av_freep(&sc->index_ranges);

        if (sc->extradata)
            for (j = 0; j < sc->stsd_count; j++)
                av_free(sc->extradata[j]);
        av_freep(&sc->extradata);
        av_freep(&sc->extradata_size);

        mov_free_encryption_index(&sc->cenc.encryption_index);
        av_encryption_info_free(sc->cenc.default_encrypted_sample);
        av_aes_ctr_free(sc->cenc.aes_ctr);

        av_freep(&sc->stereo3d);
        av_freep(&sc->spherical);
        av_freep(&sc->mastering);
        av_freep(&sc->coll);
    }

    av_freep(&mov->dv_demux);
    avformat_free_context(mov->dv_fctx);
    mov->dv_fctx = NULL;

    if (mov->meta_keys) {
        for (i = 1; i < mov->meta_keys_count; i++) {
            av_freep(&mov->meta_keys[i]);
        }
        av_freep(&mov->meta_keys);
    }

    av_freep(&mov->trex_data);
    av_freep(&mov->bitrates);

    for (i = 0; i < mov->frag_index.nb_items; i++) {
        MOVFragmentStreamInfo *frag = mov->frag_index.item[i].stream_info;
        for (j = 0; j < mov->frag_index.item[i].nb_stream_info; j++) {
            mov_free_encryption_index(&frag[j].encryption_index);
        }
        av_freep(&mov->frag_index.item[i].stream_info);
    }
    av_freep(&mov->frag_index.item);

    av_freep(&mov->aes_decrypt);
    av_freep(&mov->chapter_tracks);
    av_freep(&mov->avif_info);

    return 0;
}