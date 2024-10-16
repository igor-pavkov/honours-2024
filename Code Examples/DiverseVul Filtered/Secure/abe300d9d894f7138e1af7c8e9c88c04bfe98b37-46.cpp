static int virtio_net_set_vnet_endian_one(VirtIODevice *vdev,
                                          NetClientState *peer,
                                          bool enable)
{
    if (virtio_is_big_endian(vdev)) {
        return qemu_set_vnet_be(peer, enable);
    } else {
        return qemu_set_vnet_le(peer, enable);
    }
}