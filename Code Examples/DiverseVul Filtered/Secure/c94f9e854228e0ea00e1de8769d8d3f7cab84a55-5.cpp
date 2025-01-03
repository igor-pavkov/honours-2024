void av_dynarray_add(void *tab_ptr, int *nb_ptr, void *elem)
{
    /* see similar ffmpeg.c:grow_array() */
    int nb, nb_alloc;
    intptr_t *tab;

    nb = *nb_ptr;
    tab = *(intptr_t**)tab_ptr;
    if ((nb & (nb - 1)) == 0) {
        if (nb == 0) {
            nb_alloc = 1;
        } else {
            if (nb > INT_MAX / (2 * sizeof(intptr_t)))
                goto fail;
            nb_alloc = nb * 2;
        }
        tab = av_realloc(tab, nb_alloc * sizeof(intptr_t));
        if (!tab)
            goto fail;
        *(intptr_t**)tab_ptr = tab;
    }
    tab[nb++] = (intptr_t)elem;
    *nb_ptr = nb;
    return;

fail:
    av_freep(tab_ptr);
    *nb_ptr = 0;
}