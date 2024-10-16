static void rxfilter_notify(NetClientState *nc)
{
    VirtIONet *n = qemu_get_nic_opaque(nc);

    if (nc->rxfilter_notify_enabled) {
        char *path = object_get_canonical_path(OBJECT(n->qdev));
        qapi_event_send_nic_rx_filter_changed(!!n->netclient_name,
                                              n->netclient_name, path);
        g_free(path);

        /* disable event notification to avoid events flooding */
        nc->rxfilter_notify_enabled = 0;
    }
}