static DeviceState *failover_find_primary_device(VirtIONet *n)
{
    FailoverDevice fdev = {
        .n = n,
    };

    qbus_walk_children(sysbus_get_default(), failover_set_primary, NULL,
                       NULL, NULL, &fdev);
    return fdev.dev;
}