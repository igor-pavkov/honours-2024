static uint64_t virtio_net_guest_offloads_by_features(uint32_t features)
{
    static const uint64_t guest_offloads_mask =
        (1ULL << VIRTIO_NET_F_GUEST_CSUM) |
        (1ULL << VIRTIO_NET_F_GUEST_TSO4) |
        (1ULL << VIRTIO_NET_F_GUEST_TSO6) |
        (1ULL << VIRTIO_NET_F_GUEST_ECN)  |
        (1ULL << VIRTIO_NET_F_GUEST_UFO);

    return guest_offloads_mask & features;
}