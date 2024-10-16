static void uvesafb_init_mtrr(struct fb_info *info)
{
	struct uvesafb_par *par = info->par;

	if (mtrr && !(info->fix.smem_start & (PAGE_SIZE - 1))) {
		int temp_size = info->fix.smem_len;

		int rc;

		/* Find the largest power-of-two */
		temp_size = roundup_pow_of_two(temp_size);

		/* Try and find a power of two to add */
		do {
			rc = arch_phys_wc_add(info->fix.smem_start, temp_size);
			temp_size >>= 1;
		} while (temp_size >= PAGE_SIZE && rc == -EINVAL);

		if (rc >= 0)
			par->mtrr_handle = rc;
	}
}