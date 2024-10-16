static void __exit b43_exit(void)
{
#ifdef CONFIG_B43_SSB
	ssb_driver_unregister(&b43_ssb_driver);
#endif
#ifdef CONFIG_B43_BCMA
	bcma_driver_unregister(&b43_bcma_driver);
#endif
	b43_sdio_exit();
	b43_pcmcia_exit();
	b43_debugfs_exit();
}