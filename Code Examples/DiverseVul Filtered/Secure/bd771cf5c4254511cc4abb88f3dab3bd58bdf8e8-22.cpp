static void smtc_unmap_smem(struct smtcfb_info *sfb)
{
	if (sfb && sfb->fb->screen_base) {
		if (sfb->chip_id == 0x720)
			sfb->fb->screen_base -= 0x00200000;
		iounmap(sfb->fb->screen_base);
		sfb->fb->screen_base = NULL;
	}
}