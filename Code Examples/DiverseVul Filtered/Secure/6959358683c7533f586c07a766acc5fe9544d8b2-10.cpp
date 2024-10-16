static void free_segment_list(struct playlist *pls)
{
    int i;
    for (i = 0; i < pls->n_segments; i++) {
        av_freep(&pls->segments[i]->key);
        av_freep(&pls->segments[i]->url);
        av_freep(&pls->segments[i]);
    }
    av_freep(&pls->segments);
    pls->n_segments = 0;
}