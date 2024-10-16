void vivid_fb_release_buffers(struct vivid_dev *dev)
{
	if (dev->video_vbase == NULL)
		return;

	/* Release cmap */
	if (dev->fb_info.cmap.len)
		fb_dealloc_cmap(&dev->fb_info.cmap);

	/* Release pseudo palette */
	kfree(dev->fb_info.pseudo_palette);
	kfree((void *)dev->video_vbase);
}