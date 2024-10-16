static int __init nfs4flexfilelayout_init(void)
{
	printk(KERN_INFO "%s: NFSv4 Flexfile Layout Driver Registering...\n",
	       __func__);
	return pnfs_register_layoutdriver(&flexfilelayout_type);
}