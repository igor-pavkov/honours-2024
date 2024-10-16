static int uvesafb_vbe_init(struct fb_info *info)
{
	struct uvesafb_ktask *task = NULL;
	struct uvesafb_par *par = info->par;
	int err;

	task = uvesafb_prep();
	if (!task)
		return -ENOMEM;

	err = uvesafb_vbe_getinfo(task, par);
	if (err)
		goto out;

	err = uvesafb_vbe_getmodes(task, par);
	if (err)
		goto out;

	par->nocrtc = nocrtc;
#ifdef CONFIG_X86_32
	par->pmi_setpal = pmi_setpal;
	par->ypan = ypan;

	if (par->pmi_setpal || par->ypan) {
		if (__supported_pte_mask & _PAGE_NX) {
			par->pmi_setpal = par->ypan = 0;
			pr_warn("NX protection is active, better not use the PMI\n");
		} else {
			uvesafb_vbe_getpmi(task, par);
		}
	}
#else
	/* The protected mode interface is not available on non-x86. */
	par->pmi_setpal = par->ypan = 0;
#endif

	INIT_LIST_HEAD(&info->modelist);
	uvesafb_vbe_getmonspecs(task, info);
	uvesafb_vbe_getstatesize(task, par);

out:	uvesafb_free(task);
	return err;
}