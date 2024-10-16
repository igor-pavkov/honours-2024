static int we_want_redo(int desired_ndx)
{
	static int redo_ndx = -1;

	while (redo_ndx < desired_ndx) {
		if (redo_ndx >= 0)
			no_batched_update(redo_ndx, True);
		if ((redo_ndx = flist_ndx_pop(&batch_redo_list)) < 0)
			return 0;
	}

	if (redo_ndx == desired_ndx) {
		redo_ndx = -1;
		return 1;
	}

	return 0;
}