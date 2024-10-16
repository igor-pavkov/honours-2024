static void ff_layout_commit_release(void *data)
{
	struct nfs_commit_data *cdata = data;

	ff_layout_commit_record_layoutstats_done(&cdata->task, cdata);
	pnfs_generic_commit_release(data);
}