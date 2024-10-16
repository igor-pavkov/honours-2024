static void failover_add_primary(VirtIONet *n, Error **errp)
{
    Error *err = NULL;
    DeviceState *dev = failover_find_primary_device(n);

    if (dev) {
        return;
    }

    if (!n->primary_opts) {
        error_setg(errp, "Primary device not found");
        error_append_hint(errp, "Virtio-net failover will not work. Make "
                          "sure primary device has parameter"
                          " failover_pair_id=%s\n", n->netclient_name);
        return;
    }

    dev = qdev_device_add_from_qdict(n->primary_opts,
                                     n->primary_opts_from_json,
                                     &err);
    if (err) {
        qobject_unref(n->primary_opts);
        n->primary_opts = NULL;
    } else {
        object_unref(OBJECT(dev));
    }
    error_propagate(errp, err);
}