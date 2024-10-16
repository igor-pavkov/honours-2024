static void virtio_net_hdr_swap(VirtIODevice *vdev, struct virtio_net_hdr *hdr)
{
    virtio_tswap16s(vdev, &hdr->hdr_len);
    virtio_tswap16s(vdev, &hdr->gso_size);
    virtio_tswap16s(vdev, &hdr->csum_start);
    virtio_tswap16s(vdev, &hdr->csum_offset);
}