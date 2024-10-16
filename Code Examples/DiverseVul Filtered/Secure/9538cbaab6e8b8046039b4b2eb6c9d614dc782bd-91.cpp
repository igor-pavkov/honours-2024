static int __init b43_init(void)
{
	int err;

	b43_debugfs_init();
	err = b43_pcmcia_init();
	if (err)
		goto err_dfs_exit;
	err = b43_sdio_init();
	if (err)
		goto err_pcmcia_exit;
#ifdef CONFIG_B43_BCMA
	err = bcma_driver_register(&b43_bcma_driver);
	if (err)
		goto err_sdio_exit;
#endif
#ifdef CONFIG_B43_SSB
	err = ssb_driver_register(&b43_ssb_driver);
	if (err)
		goto err_bcma_driver_exit;
#endif
	b43_print_driverinfo();

	return err;

#ifdef CONFIG_B43_SSB
err_bcma_driver_exit:
#endif
#ifdef CONFIG_B43_BCMA
	bcma_driver_unregister(&b43_bcma_driver);
err_sdio_exit:
#endif
	b43_sdio_exit();
err_pcmcia_exit:
	b43_pcmcia_exit();
err_dfs_exit:
	b43_debugfs_exit();
	return err;
}