static void b43_print_driverinfo(void)
{
	const char *feat_pci = "", *feat_pcmcia = "", *feat_nphy = "",
		   *feat_leds = "", *feat_sdio = "";

#ifdef CONFIG_B43_PCI_AUTOSELECT
	feat_pci = "P";
#endif
#ifdef CONFIG_B43_PCMCIA
	feat_pcmcia = "M";
#endif
#ifdef CONFIG_B43_PHY_N
	feat_nphy = "N";
#endif
#ifdef CONFIG_B43_LEDS
	feat_leds = "L";
#endif
#ifdef CONFIG_B43_SDIO
	feat_sdio = "S";
#endif
	printk(KERN_INFO "Broadcom 43xx driver loaded "
	       "[ Features: %s%s%s%s%s ]\n",
	       feat_pci, feat_pcmcia, feat_nphy,
	       feat_leds, feat_sdio);
}