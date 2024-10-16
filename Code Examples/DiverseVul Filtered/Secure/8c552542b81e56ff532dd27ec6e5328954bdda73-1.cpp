static inline void net_tx_pkt_sendv(struct NetTxPkt *pkt,
    NetClientState *nc, const struct iovec *iov, int iov_cnt)
{
    if (pkt->is_loopback) {
        qemu_receive_packet_iov(nc, iov, iov_cnt);
    } else {
        qemu_sendv_packet(nc, iov, iov_cnt);
    }
}