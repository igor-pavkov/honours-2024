static void free_init_section_list(struct playlist *pls)
{
    int i;
    for (i = 0; i < pls->n_init_sections; i++) {
        av_freep(&pls->init_sections[i]->url);
        av_freep(&pls->init_sections[i]);
    }
    av_freep(&pls->init_sections);
    pls->n_init_sections = 0;
}