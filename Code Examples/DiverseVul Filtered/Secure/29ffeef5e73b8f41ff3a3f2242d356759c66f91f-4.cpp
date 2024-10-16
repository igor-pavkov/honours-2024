static int alloc_scratch_buffers(H264Context *h, int linesize)
{
    int alloc_size = FFALIGN(FFABS(linesize) + 32, 32);

    if (h->bipred_scratchpad)
        return 0;

    h->bipred_scratchpad = av_malloc(16 * 6 * alloc_size);
    // edge emu needs blocksize + filter length - 1
    // (= 21x21 for  h264)
    h->edge_emu_buffer = av_mallocz(alloc_size * 2 * 21);
    h->me.scratchpad   = av_mallocz(alloc_size * 2 * 16 * 2);

    if (!h->bipred_scratchpad || !h->edge_emu_buffer || !h->me.scratchpad) {
        av_freep(&h->bipred_scratchpad);
        av_freep(&h->edge_emu_buffer);
        av_freep(&h->me.scratchpad);
        return AVERROR(ENOMEM);
    }

    h->me.temp = h->me.scratchpad;

    return 0;
}