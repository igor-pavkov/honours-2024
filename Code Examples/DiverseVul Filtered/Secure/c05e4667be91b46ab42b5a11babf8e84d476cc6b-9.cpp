static int find_snapshot_by_id_and_name(BlockDriverState *bs,
                                        const char *id,
                                        const char *name)
{
    BDRVQcowState *s = bs->opaque;
    int i;

    if (id && name) {
        for (i = 0; i < s->nb_snapshots; i++) {
            if (!strcmp(s->snapshots[i].id_str, id) &&
                !strcmp(s->snapshots[i].name, name)) {
                return i;
            }
        }
    } else if (id) {
        for (i = 0; i < s->nb_snapshots; i++) {
            if (!strcmp(s->snapshots[i].id_str, id)) {
                return i;
            }
        }
    } else if (name) {
        for (i = 0; i < s->nb_snapshots; i++) {
            if (!strcmp(s->snapshots[i].name, name)) {
                return i;
            }
        }
    }

    return -1;
}