static int uvesafb_probe(struct platform_device *dev)
{
	struct fb_info *info;
	struct vbe_mode_ib *mode = NULL;
	struct uvesafb_par *par;
	int err = 0, i;

	info = framebuffer_alloc(sizeof(*par) +	sizeof(u32) * 256, &dev->dev);
	if (!info)
		return -ENOMEM;

	par = info->par;

	err = uvesafb_vbe_init(info);
	if (err) {
		pr_err("vbe_init() failed with %d\n", err);
		goto out;
	}

	info->fbops = &uvesafb_ops;

	i = uvesafb_vbe_init_mode(info);
	if (i < 0) {
		err = -EINVAL;
		goto out;
	} else {
		mode = &par->vbe_modes[i];
	}

	if (fb_alloc_cmap(&info->cmap, 256, 0) < 0) {
		err = -ENXIO;
		goto out;
	}

	uvesafb_init_info(info, mode);

	if (!request_region(0x3c0, 32, "uvesafb")) {
		pr_err("request region 0x3c0-0x3e0 failed\n");
		err = -EIO;
		goto out_mode;
	}

	if (!request_mem_region(info->fix.smem_start, info->fix.smem_len,
				"uvesafb")) {
		pr_err("cannot reserve video memory at 0x%lx\n",
		       info->fix.smem_start);
		err = -EIO;
		goto out_reg;
	}

	uvesafb_init_mtrr(info);
	uvesafb_ioremap(info);

	if (!info->screen_base) {
		pr_err("abort, cannot ioremap 0x%x bytes of video memory at 0x%lx\n",
		       info->fix.smem_len, info->fix.smem_start);
		err = -EIO;
		goto out_mem;
	}

	platform_set_drvdata(dev, info);

	if (register_framebuffer(info) < 0) {
		pr_err("failed to register framebuffer device\n");
		err = -EINVAL;
		goto out_unmap;
	}

	pr_info("framebuffer at 0x%lx, mapped to 0x%p, using %dk, total %dk\n",
		info->fix.smem_start, info->screen_base,
		info->fix.smem_len / 1024, par->vbe_ib.total_memory * 64);
	fb_info(info, "%s frame buffer device\n", info->fix.id);

	err = sysfs_create_group(&dev->dev.kobj, &uvesafb_dev_attgrp);
	if (err != 0)
		fb_warn(info, "failed to register attributes\n");

	return 0;

out_unmap:
	iounmap(info->screen_base);
out_mem:
	release_mem_region(info->fix.smem_start, info->fix.smem_len);
out_reg:
	release_region(0x3c0, 32);
out_mode:
	if (!list_empty(&info->modelist))
		fb_destroy_modelist(&info->modelist);
	fb_destroy_modedb(info->monspecs.modedb);
	fb_dealloc_cmap(&info->cmap);
out:
	kfree(par->vbe_modes);

	framebuffer_release(info);
	return err;
}