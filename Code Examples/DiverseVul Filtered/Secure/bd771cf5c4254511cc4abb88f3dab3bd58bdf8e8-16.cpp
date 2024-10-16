static int smtc_set_par(struct fb_info *info)
{
	smtcfb_setmode(info->par);

	return 0;
}