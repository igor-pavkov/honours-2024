static int uvesafb_vbe_getinfo(struct uvesafb_ktask *task,
			       struct uvesafb_par *par)
{
	int err;

	task->t.regs.eax = 0x4f00;
	task->t.flags = TF_VBEIB;
	task->t.buf_len = sizeof(struct vbe_ib);
	task->buf = &par->vbe_ib;
	strncpy(par->vbe_ib.vbe_signature, "VBE2", 4);

	err = uvesafb_exec(task);
	if (err || (task->t.regs.eax & 0xffff) != 0x004f) {
		pr_err("Getting VBE info block failed (eax=0x%x, err=%d)\n",
		       (u32)task->t.regs.eax, err);
		return -EINVAL;
	}

	if (par->vbe_ib.vbe_version < 0x0200) {
		pr_err("Sorry, pre-VBE 2.0 cards are not supported\n");
		return -EINVAL;
	}

	if (!par->vbe_ib.mode_list_ptr) {
		pr_err("Missing mode list!\n");
		return -EINVAL;
	}

	pr_info("");

	/*
	 * Convert string pointers and the mode list pointer into
	 * usable addresses. Print informational messages about the
	 * video adapter and its vendor.
	 */
	if (par->vbe_ib.oem_vendor_name_ptr)
		pr_cont("%s, ",
			((char *)task->buf) + par->vbe_ib.oem_vendor_name_ptr);

	if (par->vbe_ib.oem_product_name_ptr)
		pr_cont("%s, ",
			((char *)task->buf) + par->vbe_ib.oem_product_name_ptr);

	if (par->vbe_ib.oem_product_rev_ptr)
		pr_cont("%s, ",
			((char *)task->buf) + par->vbe_ib.oem_product_rev_ptr);

	if (par->vbe_ib.oem_string_ptr)
		pr_cont("OEM: %s, ",
			((char *)task->buf) + par->vbe_ib.oem_string_ptr);

	pr_cont("VBE v%d.%d\n",
		(par->vbe_ib.vbe_version & 0xff00) >> 8,
		par->vbe_ib.vbe_version & 0xff);

	return 0;
}