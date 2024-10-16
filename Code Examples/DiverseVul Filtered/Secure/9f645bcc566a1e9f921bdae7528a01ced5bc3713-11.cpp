static void uvesafb_ioremap(struct fb_info *info)
{
	info->screen_base = ioremap_wc(info->fix.smem_start, info->fix.smem_len);
}