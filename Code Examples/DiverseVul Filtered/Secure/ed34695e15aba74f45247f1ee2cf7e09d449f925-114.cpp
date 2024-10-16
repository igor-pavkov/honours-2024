static void ff_layout_write_release(void *data)
{
	struct nfs_pgio_header *hdr = data;

	ff_layout_write_record_layoutstats_done(&hdr->task, hdr);
	if (test_bit(NFS_IOHDR_RESEND_PNFS, &hdr->flags)) {
		ff_layout_send_layouterror(hdr->lseg);
		ff_layout_reset_write(hdr, true);
	} else if (test_bit(NFS_IOHDR_RESEND_MDS, &hdr->flags))
		ff_layout_reset_write(hdr, false);
	pnfs_generic_rw_release(data);
}