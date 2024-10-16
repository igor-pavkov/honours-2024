void b43err(struct b43_wl *wl, const char *fmt, ...)
{
	struct va_format vaf;
	va_list args;

	if (b43_modparam_verbose < B43_VERBOSITY_ERROR)
		return;
	if (!b43_ratelimit(wl))
		return;

	va_start(args, fmt);

	vaf.fmt = fmt;
	vaf.va = &args;

	printk(KERN_ERR "b43-%s ERROR: %pV",
	       (wl && wl->hw) ? wiphy_name(wl->hw->wiphy) : "wlan", &vaf);

	va_end(args);
}