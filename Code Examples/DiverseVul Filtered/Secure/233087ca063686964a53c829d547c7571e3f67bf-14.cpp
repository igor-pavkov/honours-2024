static void floppy_rb0_cb(struct bio *bio)
{
	struct rb0_cbdata *cbdata = (struct rb0_cbdata *)bio->bi_private;
	int drive = cbdata->drive;

	if (bio->bi_status) {
		pr_info("floppy: error %d while reading block 0\n",
			bio->bi_status);
		set_bit(FD_OPEN_SHOULD_FAIL_BIT, &drive_state[drive].flags);
	}
	complete(&cbdata->complete);
}