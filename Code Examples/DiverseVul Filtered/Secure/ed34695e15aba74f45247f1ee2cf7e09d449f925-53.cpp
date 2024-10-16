static void ff_layout_read_release(void *data)
{
	struct nfs_pgio_header *hdr = data;

	ff_layout_read_record_layoutstats_done(&hdr->task, hdr);
	if (test_bit(NFS_IOHDR_RESEND_PNFS, &hdr->flags))
		ff_layout_resend_pnfs_read(hdr);
	else if (test_bit(NFS_IOHDR_RESEND_MDS, &hdr->flags))
		ff_layout_reset_read(hdr);
	pnfs_generic_rw_release(data);
}