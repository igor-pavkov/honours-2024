void smt_stat_counter(struct s_smc *smc, int stat)
{
//      BOOLEAN RingIsUp ;

	PRINTK(KERN_INFO "smt_stat_counter\n");
	switch (stat) {
	case 0:
		PRINTK(KERN_INFO "Ring operational change.\n");
		break;
	case 1:
		PRINTK(KERN_INFO "Receive fifo overflow.\n");
		smc->os.MacStat.gen.rx_errors++;
		break;
	default:
		PRINTK(KERN_INFO "Unknown status (%d).\n", stat);
		break;
	}
}				// smt_stat_counter