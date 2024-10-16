static void virtio_set_packet_hash(const uint8_t *buf, uint8_t report,
                                   uint32_t hash)
{
    struct virtio_net_hdr_v1_hash *hdr = (void *)buf;
    hdr->hash_value = hash;
    hdr->hash_report = report;
}