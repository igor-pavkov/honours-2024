static void lo_map_destroy(struct lo_map *map)
{
    g_free(map->elems);
}