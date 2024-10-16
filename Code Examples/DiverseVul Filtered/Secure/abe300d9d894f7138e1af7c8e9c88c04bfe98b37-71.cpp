static void virtio_net_set_config(VirtIODevice *vdev, const uint8_t *config)
{
    VirtIONet *n = VIRTIO_NET(vdev);
    struct virtio_net_config netcfg = {};
    NetClientState *nc = qemu_get_queue(n->nic);

    memcpy(&netcfg, config, n->config_size);

    if (!virtio_vdev_has_feature(vdev, VIRTIO_NET_F_CTRL_MAC_ADDR) &&
        !virtio_vdev_has_feature(vdev, VIRTIO_F_VERSION_1) &&
        memcmp(netcfg.mac, n->mac, ETH_ALEN)) {
        memcpy(n->mac, netcfg.mac, ETH_ALEN);
        qemu_format_nic_info_str(qemu_get_queue(n->nic), n->mac);
    }

    /*
     * Is this VDPA? No peer means not VDPA: there's no way to
     * disconnect/reconnect a VDPA peer.
     */
    if (nc->peer && nc->peer->info->type == NET_CLIENT_DRIVER_VHOST_VDPA) {
        vhost_net_set_config(get_vhost_net(nc->peer),
                             (uint8_t *)&netcfg, 0, n->config_size,
                             VHOST_SET_CONFIG_TYPE_MASTER);
      }
}