void ring_status_indication(struct s_smc *smc, u_long status)
{
	PRINTK("ring_status_indication( ");
	if (status & RS_RES15)
		PRINTK("RS_RES15 ");
	if (status & RS_HARDERROR)
		PRINTK("RS_HARDERROR ");
	if (status & RS_SOFTERROR)
		PRINTK("RS_SOFTERROR ");
	if (status & RS_BEACON)
		PRINTK("RS_BEACON ");
	if (status & RS_PATHTEST)
		PRINTK("RS_PATHTEST ");
	if (status & RS_SELFTEST)
		PRINTK("RS_SELFTEST ");
	if (status & RS_RES9)
		PRINTK("RS_RES9 ");
	if (status & RS_DISCONNECT)
		PRINTK("RS_DISCONNECT ");
	if (status & RS_RES7)
		PRINTK("RS_RES7 ");
	if (status & RS_DUPADDR)
		PRINTK("RS_DUPADDR ");
	if (status & RS_NORINGOP)
		PRINTK("RS_NORINGOP ");
	if (status & RS_VERSION)
		PRINTK("RS_VERSION ");
	if (status & RS_STUCKBYPASSS)
		PRINTK("RS_STUCKBYPASSS ");
	if (status & RS_EVENT)
		PRINTK("RS_EVENT ");
	if (status & RS_RINGOPCHANGE)
		PRINTK("RS_RINGOPCHANGE ");
	if (status & RS_RES0)
		PRINTK("RS_RES0 ");
	PRINTK("]\n");
}				// ring_status_indication