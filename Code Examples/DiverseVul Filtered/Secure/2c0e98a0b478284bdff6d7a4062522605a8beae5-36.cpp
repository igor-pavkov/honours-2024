static void mov_free(AVFormatContext *s)
{
    MOVMuxContext *mov = s->priv_data;
    int i;

    if (mov->chapter_track) {
        if (mov->tracks[mov->chapter_track].par)
            av_freep(&mov->tracks[mov->chapter_track].par->extradata);
        av_freep(&mov->tracks[mov->chapter_track].par);
    }

    for (i = 0; i < mov->nb_streams; i++) {
        if (mov->tracks[i].tag == MKTAG('r','t','p',' '))
            ff_mov_close_hinting(&mov->tracks[i]);
        else if (mov->tracks[i].tag == MKTAG('t','m','c','d') && mov->nb_meta_tmcd)
            av_freep(&mov->tracks[i].par);
        av_freep(&mov->tracks[i].cluster);
        av_freep(&mov->tracks[i].frag_info);

        if (mov->tracks[i].vos_len)
            av_freep(&mov->tracks[i].vos_data);

        ff_mov_cenc_free(&mov->tracks[i].cenc);
    }

    av_freep(&mov->tracks);
}