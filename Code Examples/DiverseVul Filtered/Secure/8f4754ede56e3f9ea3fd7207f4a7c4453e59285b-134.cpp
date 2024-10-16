BlockDriverState *bdrv_lookup_bs(const char *device,
                                 const char *node_name,
                                 Error **errp)
{
    BlockDriverState *bs = NULL;

    if (device) {
        bs = bdrv_find(device);

        if (bs) {
            return bs;
        }
    }

    if (node_name) {
        bs = bdrv_find_node(node_name);

        if (bs) {
            return bs;
        }
    }

    error_setg(errp, "Cannot find device=%s nor node_name=%s",
                     device ? device : "",
                     node_name ? node_name : "");
    return NULL;
}