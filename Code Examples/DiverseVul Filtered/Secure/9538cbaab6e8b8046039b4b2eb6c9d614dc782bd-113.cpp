void b43dbg(struct b43_wl *wl, const char *fmt, ...)
{
	struct va_format vaf;
	va_list args;

	if (b43_modparam_verbose < B43_VERBOSITY_DEBUG)
		return;

	va_start(args, fmt);

	vaf.fmt = fmt;
	vaf.va = &args;

	printk(KERN_DEBUG "b43-%s debug: %pV",
	       (wl && wl->hw) ? wiphy_name(wl->hw->wiphy) : "wlan", &vaf);

	va_end(args);
}