static inline void pcnet_tmd_load(PCNetState *s, struct pcnet_TMD *tmd,
                                  hwaddr addr)
{
    if (!BCR_SSIZE32(s)) {
        struct {
            uint32_t tbadr;
            int16_t length;
            int16_t status;
        } xda;
        s->phys_mem_read(s->dma_opaque, addr, (void *)&xda, sizeof(xda), 0);
        tmd->tbadr = le32_to_cpu(xda.tbadr) & 0xffffff;
        tmd->length = le16_to_cpu(xda.length);
        tmd->status = (le32_to_cpu(xda.tbadr) >> 16) & 0xff00;
        tmd->misc = le16_to_cpu(xda.status) << 16;
        tmd->res = 0;
    } else {
        s->phys_mem_read(s->dma_opaque, addr, (void *)tmd, sizeof(*tmd), 0);
        le32_to_cpus(&tmd->tbadr);
        le16_to_cpus((uint16_t *)&tmd->length);
        le16_to_cpus((uint16_t *)&tmd->status);
        le32_to_cpus(&tmd->misc);
        le32_to_cpus(&tmd->res);
        if (BCR_SWSTYLE(s) == 3) {
            uint32_t tmp = tmd->tbadr;
            tmd->tbadr = tmd->misc;
            tmd->misc = tmp;
        }
    }
}