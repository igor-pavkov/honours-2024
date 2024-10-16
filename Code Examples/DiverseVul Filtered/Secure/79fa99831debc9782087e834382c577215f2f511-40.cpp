static bool intel_hda_xfer(HDACodecDevice *dev, uint32_t stnr, bool output,
                           uint8_t *buf, uint32_t len)
{
    const MemTxAttrs attrs = MEMTXATTRS_UNSPECIFIED;
    HDACodecBus *bus = HDA_BUS(dev->qdev.parent_bus);
    IntelHDAState *d = container_of(bus, IntelHDAState, codecs);
    hwaddr addr;
    uint32_t s, copy, left;
    IntelHDAStream *st;
    bool irq = false;

    st = output ? d->st + 4 : d->st;
    for (s = 0; s < 4; s++) {
        if (stnr == ((st[s].ctl >> 20) & 0x0f)) {
            st = st + s;
            break;
        }
    }
    if (s == 4) {
        return false;
    }
    if (st->bpl == NULL) {
        return false;
    }

    left = len;
    s = st->bentries;
    while (left > 0 && s-- > 0) {
        copy = left;
        if (copy > st->bsize - st->lpib)
            copy = st->bsize - st->lpib;
        if (copy > st->bpl[st->be].len - st->bp)
            copy = st->bpl[st->be].len - st->bp;

        dprint(d, 3, "dma: entry %d, pos %d/%d, copy %d\n",
               st->be, st->bp, st->bpl[st->be].len, copy);

        pci_dma_rw(&d->pci, st->bpl[st->be].addr + st->bp, buf, copy, !output,
                   attrs);
        st->lpib += copy;
        st->bp += copy;
        buf += copy;
        left -= copy;

        if (st->bpl[st->be].len == st->bp) {
            /* bpl entry filled */
            if (st->bpl[st->be].flags & 0x01) {
                irq = true;
            }
            st->bp = 0;
            st->be++;
            if (st->be == st->bentries) {
                /* bpl wrap around */
                st->be = 0;
                st->lpib = 0;
            }
        }
    }
    if (d->dp_lbase & 0x01) {
        s = st - d->st;
        addr = intel_hda_addr(d->dp_lbase & ~0x01, d->dp_ubase);
        stl_le_pci_dma(&d->pci, addr + 8 * s, st->lpib, attrs);
    }
    dprint(d, 3, "dma: --\n");

    if (irq) {
        st->ctl |= (1 << 26); /* buffer completion interrupt */
        intel_hda_update_irq(d);
    }
    return true;
}