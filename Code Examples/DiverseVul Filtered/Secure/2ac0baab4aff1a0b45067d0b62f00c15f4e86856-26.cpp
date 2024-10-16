void sqfs_close(void)
{
	sqfs_decompressor_cleanup(&ctxt);
	free(ctxt.sblk);
	ctxt.sblk = NULL;
	ctxt.cur_dev = NULL;
}