static void pcnet_init(PCNetState *s)
{
    int rlen, tlen;
    uint16_t padr[3], ladrf[4], mode;
    uint32_t rdra, tdra;

    trace_pcnet_init(s, PHYSADDR(s, CSR_IADR(s)));

    if (BCR_SSIZE32(s)) {
        struct pcnet_initblk32 initblk;
        s->phys_mem_read(s->dma_opaque, PHYSADDR(s,CSR_IADR(s)),
                (uint8_t *)&initblk, sizeof(initblk), 0);
        mode = le16_to_cpu(initblk.mode);
        rlen = initblk.rlen >> 4;
        tlen = initblk.tlen >> 4;
        ladrf[0] = le16_to_cpu(initblk.ladrf[0]);
        ladrf[1] = le16_to_cpu(initblk.ladrf[1]);
        ladrf[2] = le16_to_cpu(initblk.ladrf[2]);
        ladrf[3] = le16_to_cpu(initblk.ladrf[3]);
        padr[0] = le16_to_cpu(initblk.padr[0]);
        padr[1] = le16_to_cpu(initblk.padr[1]);
        padr[2] = le16_to_cpu(initblk.padr[2]);
        rdra = le32_to_cpu(initblk.rdra);
        tdra = le32_to_cpu(initblk.tdra);
    } else {
        struct pcnet_initblk16 initblk;
        s->phys_mem_read(s->dma_opaque, PHYSADDR(s,CSR_IADR(s)),
                (uint8_t *)&initblk, sizeof(initblk), 0);
        mode = le16_to_cpu(initblk.mode);
        ladrf[0] = le16_to_cpu(initblk.ladrf[0]);
        ladrf[1] = le16_to_cpu(initblk.ladrf[1]);
        ladrf[2] = le16_to_cpu(initblk.ladrf[2]);
        ladrf[3] = le16_to_cpu(initblk.ladrf[3]);
        padr[0] = le16_to_cpu(initblk.padr[0]);
        padr[1] = le16_to_cpu(initblk.padr[1]);
        padr[2] = le16_to_cpu(initblk.padr[2]);
        rdra = le32_to_cpu(initblk.rdra);
        tdra = le32_to_cpu(initblk.tdra);
        rlen = rdra >> 29;
        tlen = tdra >> 29;
        rdra &= 0x00ffffff;
        tdra &= 0x00ffffff;
    }

    trace_pcnet_rlen_tlen(s, rlen, tlen);

    CSR_RCVRL(s) = (rlen < 9) ? (1 << rlen) : 512;
    CSR_XMTRL(s) = (tlen < 9) ? (1 << tlen) : 512;
    s->csr[ 6] = (tlen << 12) | (rlen << 8);
    s->csr[15] = mode;
    s->csr[ 8] = ladrf[0];
    s->csr[ 9] = ladrf[1];
    s->csr[10] = ladrf[2];
    s->csr[11] = ladrf[3];
    s->csr[12] = padr[0];
    s->csr[13] = padr[1];
    s->csr[14] = padr[2];
    s->rdra = PHYSADDR(s, rdra);
    s->tdra = PHYSADDR(s, tdra);

    CSR_RCVRC(s) = CSR_RCVRL(s);
    CSR_XMTRC(s) = CSR_XMTRL(s);

    trace_pcnet_ss32_rdra_tdra(s, BCR_SSIZE32(s),
                               s->rdra, CSR_RCVRL(s), s->tdra, CSR_XMTRL(s));

    s->csr[0] |= 0x0101;
    s->csr[0] &= ~0x0004;       /* clear STOP bit */

    qemu_flush_queued_packets(qemu_get_queue(s->nic));
}