void vmx_set_host_fs_gs(struct vmcs_host_state *host, u16 fs_sel, u16 gs_sel,
			unsigned long fs_base, unsigned long gs_base)
{
	if (unlikely(fs_sel != host->fs_sel)) {
		if (!(fs_sel & 7))
			vmcs_write16(HOST_FS_SELECTOR, fs_sel);
		else
			vmcs_write16(HOST_FS_SELECTOR, 0);
		host->fs_sel = fs_sel;
	}
	if (unlikely(gs_sel != host->gs_sel)) {
		if (!(gs_sel & 7))
			vmcs_write16(HOST_GS_SELECTOR, gs_sel);
		else
			vmcs_write16(HOST_GS_SELECTOR, 0);
		host->gs_sel = gs_sel;
	}
	if (unlikely(fs_base != host->fs_base)) {
		vmcs_writel(HOST_FS_BASE, fs_base);
		host->fs_base = fs_base;
	}
	if (unlikely(gs_base != host->gs_base)) {
		vmcs_writel(HOST_GS_BASE, gs_base);
		host->gs_base = gs_base;
	}
}