void bdrv_make_anon(BlockDriverState *bs)
{
    if (bs->device_name[0] != '\0') {
        QTAILQ_REMOVE(&bdrv_states, bs, device_list);
    }
    bs->device_name[0] = '\0';
    if (bs->node_name[0] != '\0') {
        QTAILQ_REMOVE(&graph_bdrv_states, bs, node_list);
    }
    bs->node_name[0] = '\0';
}