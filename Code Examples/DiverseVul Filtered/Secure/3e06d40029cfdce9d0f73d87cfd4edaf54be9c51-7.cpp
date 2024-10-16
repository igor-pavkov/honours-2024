static void no_batched_update(int ndx, BOOL is_redo)
{
	struct file_list *flist = flist_for_ndx(ndx, "no_batched_update");
	struct file_struct *file = flist->files[ndx - flist->ndx_start];

	rprintf(FERROR_XFER, "(No batched update for%s \"%s\")\n",
		is_redo ? " resend of" : "", f_name(file, NULL));

	if (inc_recurse && !dry_run)
		send_msg_int(MSG_NO_SEND, ndx);
}