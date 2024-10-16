static int uvesafb_release(struct fb_info *info, int user)
{
	struct uvesafb_ktask *task = NULL;
	struct uvesafb_par *par = info->par;
	int cnt = atomic_read(&par->ref_count);

	if (!cnt)
		return -EINVAL;

	if (cnt != 1)
		goto out;

	task = uvesafb_prep();
	if (!task)
		goto out;

	/* First, try to set the standard 80x25 text mode. */
	task->t.regs.eax = 0x0003;
	uvesafb_exec(task);

	/*
	 * Now try to restore whatever hardware state we might have
	 * saved when the fb device was first opened.
	 */
	uvesafb_vbe_state_restore(par, par->vbe_state_orig);
out:
	atomic_dec(&par->ref_count);
	uvesafb_free(task);
	return 0;
}