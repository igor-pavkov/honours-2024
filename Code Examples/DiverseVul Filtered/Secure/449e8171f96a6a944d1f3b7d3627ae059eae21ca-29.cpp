static int lo_map_grow(struct lo_map *map, size_t new_nelems)
{
    struct lo_map_elem *new_elems;
    size_t i;

    if (new_nelems <= map->nelems) {
        return 1;
    }

    new_elems = g_try_realloc_n(map->elems, new_nelems, sizeof(map->elems[0]));
    if (!new_elems) {
        return 0;
    }

    for (i = map->nelems; i < new_nelems; i++) {
        new_elems[i].freelist = i + 1;
        new_elems[i].in_use = false;
    }
    new_elems[new_nelems - 1].freelist = -1;

    map->elems = new_elems;
    map->freelist = map->nelems;
    map->nelems = new_nelems;
    return 1;
}