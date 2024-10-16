static inline uint64_t virtio_net_supported_guest_offloads(VirtIONet *n)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(n);
    return virtio_net_guest_offloads_by_features(vdev->guest_features);
}