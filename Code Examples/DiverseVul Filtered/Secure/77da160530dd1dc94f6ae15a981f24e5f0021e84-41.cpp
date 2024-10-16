static int diskstats_show(struct seq_file *seqf, void *v)
{
	struct gendisk *gp = v;
	struct disk_part_iter piter;
	struct hd_struct *hd;
	char buf[BDEVNAME_SIZE];
	int cpu;

	/*
	if (&disk_to_dev(gp)->kobj.entry == block_class.devices.next)
		seq_puts(seqf,	"major minor name"
				"     rio rmerge rsect ruse wio wmerge "
				"wsect wuse running use aveq"
				"\n\n");
	*/

	disk_part_iter_init(&piter, gp, DISK_PITER_INCL_EMPTY_PART0);
	while ((hd = disk_part_iter_next(&piter))) {
		cpu = part_stat_lock();
		part_round_stats(cpu, hd);
		part_stat_unlock();
		seq_printf(seqf, "%4d %7d %s %lu %lu %lu "
			   "%u %lu %lu %lu %u %u %u %u\n",
			   MAJOR(part_devt(hd)), MINOR(part_devt(hd)),
			   disk_name(gp, hd->partno, buf),
			   part_stat_read(hd, ios[READ]),
			   part_stat_read(hd, merges[READ]),
			   part_stat_read(hd, sectors[READ]),
			   jiffies_to_msecs(part_stat_read(hd, ticks[READ])),
			   part_stat_read(hd, ios[WRITE]),
			   part_stat_read(hd, merges[WRITE]),
			   part_stat_read(hd, sectors[WRITE]),
			   jiffies_to_msecs(part_stat_read(hd, ticks[WRITE])),
			   part_in_flight(hd),
			   jiffies_to_msecs(part_stat_read(hd, io_ticks)),
			   jiffies_to_msecs(part_stat_read(hd, time_in_queue))
			);
	}
	disk_part_iter_exit(&piter);

	return 0;
}