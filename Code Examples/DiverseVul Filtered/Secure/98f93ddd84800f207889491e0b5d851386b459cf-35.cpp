static void virtio_net_instance_init(Object *obj)
{
    VirtIONet *n = VIRTIO_NET(obj);

    /*
     * The default config_size is sizeof(struct virtio_net_config).
     * Can be overriden with virtio_net_set_config_size.
     */
    n->config_size = sizeof(struct virtio_net_config);
}