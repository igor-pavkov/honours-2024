void *av_dynarray2_add(void **tab_ptr, int *nb_ptr, size_t elem_size,
                       const uint8_t *elem_data)
{
    int nb = *nb_ptr, nb_alloc;
    uint8_t *tab = *tab_ptr, *tab_elem_data;

    if ((nb & (nb - 1)) == 0) {
        if (nb == 0) {
            nb_alloc = 1;
        } else {
            if (nb > INT_MAX / (2 * elem_size))
                goto fail;
            nb_alloc = nb * 2;
        }
        tab = av_realloc(tab, nb_alloc * elem_size);
        if (!tab)
            goto fail;
        *tab_ptr = tab;
    }
    *nb_ptr = nb + 1;
    tab_elem_data = tab + nb*elem_size;
    if (elem_data)
        memcpy(tab_elem_data, elem_data, elem_size);
    else if (CONFIG_MEMORY_POISONING)
        memset(tab_elem_data, FF_MEMORY_POISON, elem_size);
    return tab_elem_data;

fail:
    av_freep(tab_ptr);
    *nb_ptr = 0;
    return NULL;
}