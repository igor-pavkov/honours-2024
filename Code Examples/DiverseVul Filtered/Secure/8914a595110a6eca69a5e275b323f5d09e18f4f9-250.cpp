void bnx2x_fw_dump_lvl(struct bnx2x *bp, const char *lvl)
{
	u32 addr, val;
	u32 mark, offset;
	__be32 data[9];
	int word;
	u32 trace_shmem_base;
	if (BP_NOMCP(bp)) {
		BNX2X_ERR("NO MCP - can not dump\n");
		return;
	}
	netdev_printk(lvl, bp->dev, "bc %d.%d.%d\n",
		(bp->common.bc_ver & 0xff0000) >> 16,
		(bp->common.bc_ver & 0xff00) >> 8,
		(bp->common.bc_ver & 0xff));

	if (pci_channel_offline(bp->pdev)) {
		BNX2X_ERR("Cannot dump MCP info while in PCI error\n");
		return;
	}

	val = REG_RD(bp, MCP_REG_MCPR_CPU_PROGRAM_COUNTER);
	if (val == REG_RD(bp, MCP_REG_MCPR_CPU_PROGRAM_COUNTER))
		BNX2X_ERR("%s" "MCP PC at 0x%x\n", lvl, val);

	if (BP_PATH(bp) == 0)
		trace_shmem_base = bp->common.shmem_base;
	else
		trace_shmem_base = SHMEM2_RD(bp, other_shmem_base_addr);

	/* sanity */
	if (trace_shmem_base < MCPR_SCRATCH_BASE(bp) + MCPR_TRACE_BUFFER_SIZE ||
	    trace_shmem_base >= MCPR_SCRATCH_BASE(bp) +
				SCRATCH_BUFFER_SIZE(bp)) {
		BNX2X_ERR("Unable to dump trace buffer (mark %x)\n",
			  trace_shmem_base);
		return;
	}

	addr = trace_shmem_base - MCPR_TRACE_BUFFER_SIZE;

	/* validate TRCB signature */
	mark = REG_RD(bp, addr);
	if (mark != MFW_TRACE_SIGNATURE) {
		BNX2X_ERR("Trace buffer signature is missing.");
		return ;
	}

	/* read cyclic buffer pointer */
	addr += 4;
	mark = REG_RD(bp, addr);
	mark = MCPR_SCRATCH_BASE(bp) + ((mark + 0x3) & ~0x3) - 0x08000000;
	if (mark >= trace_shmem_base || mark < addr + 4) {
		BNX2X_ERR("Mark doesn't fall inside Trace Buffer\n");
		return;
	}
	printk("%s" "begin fw dump (mark 0x%x)\n", lvl, mark);

	printk("%s", lvl);

	/* dump buffer after the mark */
	for (offset = mark; offset < trace_shmem_base; offset += 0x8*4) {
		for (word = 0; word < 8; word++)
			data[word] = htonl(REG_RD(bp, offset + 4*word));
		data[8] = 0x0;
		pr_cont("%s", (char *)data);
	}

	/* dump buffer before the mark */
	for (offset = addr + 4; offset <= mark; offset += 0x8*4) {
		for (word = 0; word < 8; word++)
			data[word] = htonl(REG_RD(bp, offset + 4*word));
		data[8] = 0x0;
		pr_cont("%s", (char *)data);
	}
	printk("%s" "end of fw dump\n", lvl);
}