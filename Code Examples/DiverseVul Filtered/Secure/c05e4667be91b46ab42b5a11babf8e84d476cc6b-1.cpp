void qcow2_free_snapshots(BlockDriverState *bs)
{
    BDRVQcowState *s = bs->opaque;
    int i;

    for(i = 0; i < s->nb_snapshots; i++) {
        g_free(s->snapshots[i].name);
        g_free(s->snapshots[i].id_str);
    }
    g_free(s->snapshots);
    s->snapshots = NULL;
    s->nb_snapshots = 0;
}