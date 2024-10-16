static void work_around_broken_dhclient(struct virtio_net_hdr *hdr,
                                        uint8_t *buf, size_t size)
{
    if ((hdr->flags & VIRTIO_NET_HDR_F_NEEDS_CSUM) && /* missing csum */
        (size > 27 && size < 1500) && /* normal sized MTU */
        (buf[12] == 0x08 && buf[13] == 0x00) && /* ethertype == IPv4 */
        (buf[23] == 17) && /* ip.protocol == UDP */
        (buf[34] == 0 && buf[35] == 67)) { /* udp.srcport == bootps */
        net_checksum_calculate(buf, size, CSUM_UDP);
        hdr->flags &= ~VIRTIO_NET_HDR_F_NEEDS_CSUM;
    }
}