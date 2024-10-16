void saa7164_bus_dump(struct saa7164_dev *dev)
{
	struct tmComResBusInfo *b = &dev->bus;

	dprintk(DBGLVL_BUS, "Dumping the bus structure:\n");
	dprintk(DBGLVL_BUS, " .type             = %d\n", b->Type);
	dprintk(DBGLVL_BUS, " .dev->bmmio       = 0x%p\n", dev->bmmio);
	dprintk(DBGLVL_BUS, " .m_wMaxReqSize    = 0x%x\n", b->m_wMaxReqSize);
	dprintk(DBGLVL_BUS, " .m_pdwSetRing     = 0x%p\n", b->m_pdwSetRing);
	dprintk(DBGLVL_BUS, " .m_dwSizeSetRing  = 0x%x\n", b->m_dwSizeSetRing);
	dprintk(DBGLVL_BUS, " .m_pdwGetRing     = 0x%p\n", b->m_pdwGetRing);
	dprintk(DBGLVL_BUS, " .m_dwSizeGetRing  = 0x%x\n", b->m_dwSizeGetRing);

	dprintk(DBGLVL_BUS, " .m_dwSetReadPos   = 0x%x (0x%08x)\n",
		b->m_dwSetReadPos, saa7164_readl(b->m_dwSetReadPos));

	dprintk(DBGLVL_BUS, " .m_dwSetWritePos  = 0x%x (0x%08x)\n",
		b->m_dwSetWritePos, saa7164_readl(b->m_dwSetWritePos));

	dprintk(DBGLVL_BUS, " .m_dwGetReadPos   = 0x%x (0x%08x)\n",
		b->m_dwGetReadPos, saa7164_readl(b->m_dwGetReadPos));

	dprintk(DBGLVL_BUS, " .m_dwGetWritePos  = 0x%x (0x%08x)\n",
		b->m_dwGetWritePos, saa7164_readl(b->m_dwGetWritePos));

}