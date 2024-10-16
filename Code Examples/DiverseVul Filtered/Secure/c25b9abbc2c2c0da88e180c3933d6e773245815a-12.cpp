void drv_reset_indication(struct s_smc *smc)
{
	PRINTK(KERN_INFO "entering drv_reset_indication\n");

	smc->os.ResetRequested = TRUE;	// Set flag.

}				// drv_reset_indication