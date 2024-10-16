static int uvesafb_remove(struct platform_device *dev)
{
	struct fb_info *info = platform_get_drvdata(dev);

	if (info) {
		struct uvesafb_par *par = info->par;

		sysfs_remove_group(&dev->dev.kobj, &uvesafb_dev_attgrp);
		unregister_framebuffer(info);
		release_region(0x3c0, 32);
		iounmap(info->screen_base);
		arch_phys_wc_del(par->mtrr_handle);
		release_mem_region(info->fix.smem_start, info->fix.smem_len);
		fb_destroy_modedb(info->monspecs.modedb);
		fb_dealloc_cmap(&info->cmap);

		kfree(par->vbe_modes);
		kfree(par->vbe_state_orig);
		kfree(par->vbe_state_saved);

		framebuffer_release(info);
	}
	return 0;
}