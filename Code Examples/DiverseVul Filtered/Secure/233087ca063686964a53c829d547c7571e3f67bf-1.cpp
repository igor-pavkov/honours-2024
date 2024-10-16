static int __init do_floppy_init(void)
{
	int i, unit, drive, err;

	set_debugt();
	interruptjiffies = resultjiffies = jiffies;

#if defined(CONFIG_PPC)
	if (check_legacy_ioport(FDC1))
		return -ENODEV;
#endif

	raw_cmd = NULL;

	floppy_wq = alloc_ordered_workqueue("floppy", 0);
	if (!floppy_wq)
		return -ENOMEM;

	for (drive = 0; drive < N_DRIVE; drive++) {
		memset(&tag_sets[drive], 0, sizeof(tag_sets[drive]));
		tag_sets[drive].ops = &floppy_mq_ops;
		tag_sets[drive].nr_hw_queues = 1;
		tag_sets[drive].nr_maps = 1;
		tag_sets[drive].queue_depth = 2;
		tag_sets[drive].numa_node = NUMA_NO_NODE;
		tag_sets[drive].flags = BLK_MQ_F_SHOULD_MERGE;
		err = blk_mq_alloc_tag_set(&tag_sets[drive]);
		if (err)
			goto out_put_disk;

		err = floppy_alloc_disk(drive, 0);
		if (err)
			goto out_put_disk;

		timer_setup(&motor_off_timer[drive], motor_off_callback, 0);
	}

	err = __register_blkdev(FLOPPY_MAJOR, "fd", floppy_probe);
	if (err)
		goto out_put_disk;

	err = platform_driver_register(&floppy_driver);
	if (err)
		goto out_unreg_blkdev;

	for (i = 0; i < 256; i++)
		if (ITYPE(i))
			floppy_sizes[i] = floppy_type[ITYPE(i)].size;
		else
			floppy_sizes[i] = MAX_DISK_SIZE << 1;

	reschedule_timeout(MAXTIMEOUT, "floppy init");
	config_types();

	for (i = 0; i < N_FDC; i++) {
		memset(&fdc_state[i], 0, sizeof(*fdc_state));
		fdc_state[i].dtr = -1;
		fdc_state[i].dor = 0x4;
#if defined(__sparc__) || defined(__mc68000__)
	/*sparcs/sun3x don't have a DOR reset which we can fall back on to */
#ifdef __mc68000__
		if (MACH_IS_SUN3X)
#endif
			fdc_state[i].version = FDC_82072A;
#endif
	}

	use_virtual_dma = can_use_virtual_dma & 1;
	fdc_state[0].address = FDC1;
	if (fdc_state[0].address == -1) {
		cancel_delayed_work(&fd_timeout);
		err = -ENODEV;
		goto out_unreg_driver;
	}
#if N_FDC > 1
	fdc_state[1].address = FDC2;
#endif

	current_fdc = 0;	/* reset fdc in case of unexpected interrupt */
	err = floppy_grab_irq_and_dma();
	if (err) {
		cancel_delayed_work(&fd_timeout);
		err = -EBUSY;
		goto out_unreg_driver;
	}

	/* initialise drive state */
	for (drive = 0; drive < N_DRIVE; drive++) {
		memset(&drive_state[drive], 0, sizeof(drive_state[drive]));
		memset(&write_errors[drive], 0, sizeof(write_errors[drive]));
		set_bit(FD_DISK_NEWCHANGE_BIT, &drive_state[drive].flags);
		set_bit(FD_DISK_CHANGED_BIT, &drive_state[drive].flags);
		set_bit(FD_VERIFY_BIT, &drive_state[drive].flags);
		drive_state[drive].fd_device = -1;
		floppy_track_buffer = NULL;
		max_buffer_sectors = 0;
	}
	/*
	 * Small 10 msec delay to let through any interrupt that
	 * initialization might have triggered, to not
	 * confuse detection:
	 */
	msleep(10);

	for (i = 0; i < N_FDC; i++) {
		fdc_state[i].driver_version = FD_DRIVER_VERSION;
		for (unit = 0; unit < 4; unit++)
			fdc_state[i].track[unit] = 0;
		if (fdc_state[i].address == -1)
			continue;
		fdc_state[i].rawcmd = 2;
		if (user_reset_fdc(REVDRIVE(i, 0), FD_RESET_ALWAYS, false)) {
			/* free ioports reserved by floppy_grab_irq_and_dma() */
			floppy_release_regions(i);
			fdc_state[i].address = -1;
			fdc_state[i].version = FDC_NONE;
			continue;
		}
		/* Try to determine the floppy controller type */
		fdc_state[i].version = get_fdc_version(i);
		if (fdc_state[i].version == FDC_NONE) {
			/* free ioports reserved by floppy_grab_irq_and_dma() */
			floppy_release_regions(i);
			fdc_state[i].address = -1;
			continue;
		}
		if (can_use_virtual_dma == 2 &&
		    fdc_state[i].version < FDC_82072A)
			can_use_virtual_dma = 0;

		have_no_fdc = 0;
		/* Not all FDCs seem to be able to handle the version command
		 * properly, so force a reset for the standard FDC clones,
		 * to avoid interrupt garbage.
		 */
		user_reset_fdc(REVDRIVE(i, 0), FD_RESET_ALWAYS, false);
	}
	current_fdc = 0;
	cancel_delayed_work(&fd_timeout);
	current_drive = 0;
	initialized = true;
	if (have_no_fdc) {
		DPRINT("no floppy controllers found\n");
		err = have_no_fdc;
		goto out_release_dma;
	}

	for (drive = 0; drive < N_DRIVE; drive++) {
		if (!floppy_available(drive))
			continue;

		floppy_device[drive].name = floppy_device_name;
		floppy_device[drive].id = drive;
		floppy_device[drive].dev.release = floppy_device_release;
		floppy_device[drive].dev.groups = floppy_dev_groups;

		err = platform_device_register(&floppy_device[drive]);
		if (err)
			goto out_remove_drives;

		registered[drive] = true;

		err = device_add_disk(&floppy_device[drive].dev,
				      disks[drive][0], NULL);
		if (err)
			goto out_remove_drives;
	}

	return 0;

out_remove_drives:
	while (drive--) {
		if (floppy_available(drive)) {
			del_gendisk(disks[drive][0]);
			if (registered[drive])
				platform_device_unregister(&floppy_device[drive]);
		}
	}
out_release_dma:
	if (atomic_read(&usage_count))
		floppy_release_irq_and_dma();
out_unreg_driver:
	platform_driver_unregister(&floppy_driver);
out_unreg_blkdev:
	unregister_blkdev(FLOPPY_MAJOR, "fd");
out_put_disk:
	destroy_workqueue(floppy_wq);
	for (drive = 0; drive < N_DRIVE; drive++) {
		if (!disks[drive][0])
			break;
		del_timer_sync(&motor_off_timer[drive]);
		blk_cleanup_disk(disks[drive][0]);
		blk_mq_free_tag_set(&tag_sets[drive]);
	}
	return err;
}