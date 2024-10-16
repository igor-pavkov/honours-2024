static void __exit floppy_module_exit(void)
{
	int drive, i;

	unregister_blkdev(FLOPPY_MAJOR, "fd");
	platform_driver_unregister(&floppy_driver);

	destroy_workqueue(floppy_wq);

	for (drive = 0; drive < N_DRIVE; drive++) {
		del_timer_sync(&motor_off_timer[drive]);

		if (floppy_available(drive)) {
			for (i = 0; i < ARRAY_SIZE(floppy_type); i++) {
				if (disks[drive][i])
					del_gendisk(disks[drive][i]);
			}
			if (registered[drive])
				platform_device_unregister(&floppy_device[drive]);
		}
		for (i = 0; i < ARRAY_SIZE(floppy_type); i++) {
			if (disks[drive][i])
				blk_cleanup_disk(disks[drive][i]);
		}
		blk_mq_free_tag_set(&tag_sets[drive]);
	}

	cancel_delayed_work_sync(&fd_timeout);
	cancel_delayed_work_sync(&fd_timer);

	if (atomic_read(&usage_count))
		floppy_release_irq_and_dma();

	/* eject disk, if any */
	fd_eject(0);
}