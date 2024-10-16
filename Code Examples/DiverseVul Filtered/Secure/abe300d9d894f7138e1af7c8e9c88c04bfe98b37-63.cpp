static uint64_t virtio_net_bad_features(VirtIODevice *vdev)
{
    uint64_t features = 0;

    /* Linux kernel 2.6.25.  It understood MAC (as everyone must),
     * but also these: */
    virtio_add_feature(&features, VIRTIO_NET_F_MAC);
    virtio_add_feature(&features, VIRTIO_NET_F_CSUM);
    virtio_add_feature(&features, VIRTIO_NET_F_HOST_TSO4);
    virtio_add_feature(&features, VIRTIO_NET_F_HOST_TSO6);
    virtio_add_feature(&features, VIRTIO_NET_F_HOST_ECN);

    return features;
}