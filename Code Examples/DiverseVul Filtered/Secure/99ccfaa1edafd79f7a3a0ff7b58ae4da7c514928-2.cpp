static void pcnet_start(PCNetState *s)
{
#ifdef PCNET_DEBUG
    printf("pcnet_start\n");
#endif

    if (!CSR_DTX(s)) {
        s->csr[0] |= 0x0010;    /* set TXON */
    }
    if (!CSR_DRX(s)) {
        s->csr[0] |= 0x0020;    /* set RXON */
    }
    s->csr[0] &= ~0x0004;       /* clear STOP bit */
    s->csr[0] |= 0x0002;
    pcnet_poll_timer(s);

    qemu_flush_queued_packets(qemu_get_queue(s->nic));
}