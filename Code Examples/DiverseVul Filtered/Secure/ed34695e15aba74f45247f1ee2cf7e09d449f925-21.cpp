static void ff_layout_resend_pnfs_read(struct nfs_pgio_header *hdr)
{
	u32 idx = hdr->pgio_mirror_idx + 1;
	u32 new_idx = 0;

	if (ff_layout_choose_any_ds_for_read(hdr->lseg, idx, &new_idx))
		ff_layout_send_layouterror(hdr->lseg);
	else
		pnfs_error_mark_layout_for_return(hdr->inode, hdr->lseg);
	pnfs_read_resend_pnfs(hdr, new_idx);
}