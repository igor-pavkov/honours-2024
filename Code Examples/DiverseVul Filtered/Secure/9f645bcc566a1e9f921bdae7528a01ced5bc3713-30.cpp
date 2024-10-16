static int uvesafb_pan_display(struct fb_var_screeninfo *var,
		struct fb_info *info)
{
#ifdef CONFIG_X86_32
	int offset;
	struct uvesafb_par *par = info->par;

	offset = (var->yoffset * info->fix.line_length + var->xoffset) / 4;

	/*
	 * It turns out it's not the best idea to do panning via vm86,
	 * so we only allow it if we have a PMI.
	 */
	if (par->pmi_start) {
		__asm__ __volatile__(
			"call *(%%edi)"
			: /* no return value */
			: "a" (0x4f07),         /* EAX */
			  "b" (0),              /* EBX */
			  "c" (offset),         /* ECX */
			  "d" (offset >> 16),   /* EDX */
			  "D" (&par->pmi_start));    /* EDI */
	}
#endif
	return 0;
}