static void __exit nfs4flexfilelayout_exit(void)
{
	printk(KERN_INFO "%s: NFSv4 Flexfile Layout Driver Unregistering...\n",
	       __func__);
	pnfs_unregister_layoutdriver(&flexfilelayout_type);
}