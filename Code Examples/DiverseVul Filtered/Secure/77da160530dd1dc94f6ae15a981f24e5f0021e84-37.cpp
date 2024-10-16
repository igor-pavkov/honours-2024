static int __init proc_genhd_init(void)
{
	proc_create("diskstats", 0, NULL, &proc_diskstats_operations);
	proc_create("partitions", 0, NULL, &proc_partitions_operations);
	return 0;
}