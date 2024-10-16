void virtio_net_set_netclient_name(VirtIONet *n, const char *name,
                                   const char *type)
{
    /*
     * The name can be NULL, the netclient name will be type.x.
     */
    assert(type != NULL);

    if (n->netclient_name) {
        g_free(n->netclient_name);
        n->netclient_name = NULL;
    }
    if (n->netclient_type) {
        g_free(n->netclient_type);
        n->netclient_type = NULL;
    }

    if (name != NULL) {
        n->netclient_name = g_strdup(name);
    }
    n->netclient_type = g_strdup(type);
}