static int f2fs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct f2fs_sb_info *sbi;
	struct f2fs_super_block *raw_super;
	struct inode *root;
	int err;
	bool retry = true, need_fsck = false;
	char *options = NULL;
	int recovery, i, valid_super_block;
	struct curseg_info *seg_i;

try_onemore:
	err = -EINVAL;
	raw_super = NULL;
	valid_super_block = -1;
	recovery = 0;

	/* allocate memory for f2fs-specific super block info */
	sbi = kzalloc(sizeof(struct f2fs_sb_info), GFP_KERNEL);
	if (!sbi)
		return -ENOMEM;

	sbi->sb = sb;

	/* Load the checksum driver */
	sbi->s_chksum_driver = crypto_alloc_shash("crc32", 0, 0);
	if (IS_ERR(sbi->s_chksum_driver)) {
		f2fs_msg(sb, KERN_ERR, "Cannot load crc32 driver.");
		err = PTR_ERR(sbi->s_chksum_driver);
		sbi->s_chksum_driver = NULL;
		goto free_sbi;
	}

	/* set a block size */
	if (unlikely(!sb_set_blocksize(sb, F2FS_BLKSIZE))) {
		f2fs_msg(sb, KERN_ERR, "unable to set blocksize");
		goto free_sbi;
	}

	err = read_raw_super_block(sbi, &raw_super, &valid_super_block,
								&recovery);
	if (err)
		goto free_sbi;

	sb->s_fs_info = sbi;
	sbi->raw_super = raw_super;

	/*
	 * The BLKZONED feature indicates that the drive was formatted with
	 * zone alignment optimization. This is optional for host-aware
	 * devices, but mandatory for host-managed zoned block devices.
	 */
#ifndef CONFIG_BLK_DEV_ZONED
	if (f2fs_sb_mounted_blkzoned(sb)) {
		f2fs_msg(sb, KERN_ERR,
			 "Zoned block device support is not enabled\n");
		goto free_sb_buf;
	}
#endif
	default_options(sbi);
	/* parse mount options */
	options = kstrdup((const char *)data, GFP_KERNEL);
	if (data && !options) {
		err = -ENOMEM;
		goto free_sb_buf;
	}

	err = parse_options(sb, options);
	if (err)
		goto free_options;

	sbi->max_file_blocks = max_file_blocks();
	sb->s_maxbytes = sbi->max_file_blocks <<
				le32_to_cpu(raw_super->log_blocksize);
	sb->s_max_links = F2FS_LINK_MAX;
	get_random_bytes(&sbi->s_next_generation, sizeof(u32));

	sb->s_op = &f2fs_sops;
	sb->s_cop = &f2fs_cryptops;
	sb->s_xattr = f2fs_xattr_handlers;
	sb->s_export_op = &f2fs_export_ops;
	sb->s_magic = F2FS_SUPER_MAGIC;
	sb->s_time_gran = 1;
	sb->s_flags = (sb->s_flags & ~MS_POSIXACL) |
		(test_opt(sbi, POSIX_ACL) ? MS_POSIXACL : 0);
	memcpy(sb->s_uuid, raw_super->uuid, sizeof(raw_super->uuid));

	/* init f2fs-specific super block info */
	sbi->valid_super_block = valid_super_block;
	mutex_init(&sbi->gc_mutex);
	mutex_init(&sbi->cp_mutex);
	init_rwsem(&sbi->node_write);
	init_rwsem(&sbi->node_change);

	/* disallow all the data/node/meta page writes */
	set_sbi_flag(sbi, SBI_POR_DOING);
	spin_lock_init(&sbi->stat_lock);

	init_rwsem(&sbi->read_io.io_rwsem);
	sbi->read_io.sbi = sbi;
	sbi->read_io.bio = NULL;
	for (i = 0; i < NR_PAGE_TYPE; i++) {
		init_rwsem(&sbi->write_io[i].io_rwsem);
		sbi->write_io[i].sbi = sbi;
		sbi->write_io[i].bio = NULL;
	}

	init_rwsem(&sbi->cp_rwsem);
	init_waitqueue_head(&sbi->cp_wait);
	init_sb_info(sbi);

	err = init_percpu_info(sbi);
	if (err)
		goto free_options;

	if (F2FS_IO_SIZE(sbi) > 1) {
		sbi->write_io_dummy =
			mempool_create_page_pool(2 * (F2FS_IO_SIZE(sbi) - 1), 0);
		if (!sbi->write_io_dummy)
			goto free_options;
	}

	/* get an inode for meta space */
	sbi->meta_inode = f2fs_iget(sb, F2FS_META_INO(sbi));
	if (IS_ERR(sbi->meta_inode)) {
		f2fs_msg(sb, KERN_ERR, "Failed to read F2FS meta data inode");
		err = PTR_ERR(sbi->meta_inode);
		goto free_io_dummy;
	}

	err = get_valid_checkpoint(sbi);
	if (err) {
		f2fs_msg(sb, KERN_ERR, "Failed to get valid F2FS checkpoint");
		goto free_meta_inode;
	}

	/* Initialize device list */
	err = f2fs_scan_devices(sbi);
	if (err) {
		f2fs_msg(sb, KERN_ERR, "Failed to find devices");
		goto free_devices;
	}

	sbi->total_valid_node_count =
				le32_to_cpu(sbi->ckpt->valid_node_count);
	percpu_counter_set(&sbi->total_valid_inode_count,
				le32_to_cpu(sbi->ckpt->valid_inode_count));
	sbi->user_block_count = le64_to_cpu(sbi->ckpt->user_block_count);
	sbi->total_valid_block_count =
				le64_to_cpu(sbi->ckpt->valid_block_count);
	sbi->last_valid_block_count = sbi->total_valid_block_count;

	for (i = 0; i < NR_INODE_TYPE; i++) {
		INIT_LIST_HEAD(&sbi->inode_list[i]);
		spin_lock_init(&sbi->inode_lock[i]);
	}

	init_extent_cache_info(sbi);

	init_ino_entry_info(sbi);

	/* setup f2fs internal modules */
	err = build_segment_manager(sbi);
	if (err) {
		f2fs_msg(sb, KERN_ERR,
			"Failed to initialize F2FS segment manager");
		goto free_sm;
	}
	err = build_node_manager(sbi);
	if (err) {
		f2fs_msg(sb, KERN_ERR,
			"Failed to initialize F2FS node manager");
		goto free_nm;
	}

	/* For write statistics */
	if (sb->s_bdev->bd_part)
		sbi->sectors_written_start =
			(u64)part_stat_read(sb->s_bdev->bd_part, sectors[1]);

	/* Read accumulated write IO statistics if exists */
	seg_i = CURSEG_I(sbi, CURSEG_HOT_NODE);
	if (__exist_node_summaries(sbi))
		sbi->kbytes_written =
			le64_to_cpu(seg_i->journal->info.kbytes_written);

	build_gc_manager(sbi);

	/* get an inode for node space */
	sbi->node_inode = f2fs_iget(sb, F2FS_NODE_INO(sbi));
	if (IS_ERR(sbi->node_inode)) {
		f2fs_msg(sb, KERN_ERR, "Failed to read node inode");
		err = PTR_ERR(sbi->node_inode);
		goto free_nm;
	}

	f2fs_join_shrinker(sbi);

	err = f2fs_build_stats(sbi);
	if (err)
		goto free_nm;

	/* if there are nt orphan nodes free them */
	err = recover_orphan_inodes(sbi);
	if (err)
		goto free_node_inode;

	/* read root inode and dentry */
	root = f2fs_iget(sb, F2FS_ROOT_INO(sbi));
	if (IS_ERR(root)) {
		f2fs_msg(sb, KERN_ERR, "Failed to read root inode");
		err = PTR_ERR(root);
		goto free_node_inode;
	}
	if (!S_ISDIR(root->i_mode) || !root->i_blocks || !root->i_size) {
		iput(root);
		err = -EINVAL;
		goto free_node_inode;
	}

	sb->s_root = d_make_root(root); /* allocate root dentry */
	if (!sb->s_root) {
		err = -ENOMEM;
		goto free_root_inode;
	}

	if (f2fs_proc_root)
		sbi->s_proc = proc_mkdir(sb->s_id, f2fs_proc_root);

	if (sbi->s_proc) {
		proc_create_data("segment_info", S_IRUGO, sbi->s_proc,
				 &f2fs_seq_segment_info_fops, sb);
		proc_create_data("segment_bits", S_IRUGO, sbi->s_proc,
				 &f2fs_seq_segment_bits_fops, sb);
	}

	sbi->s_kobj.kset = f2fs_kset;
	init_completion(&sbi->s_kobj_unregister);
	err = kobject_init_and_add(&sbi->s_kobj, &f2fs_ktype, NULL,
							"%s", sb->s_id);
	if (err)
		goto free_proc;

	/* recover fsynced data */
	if (!test_opt(sbi, DISABLE_ROLL_FORWARD)) {
		/*
		 * mount should be failed, when device has readonly mode, and
		 * previous checkpoint was not done by clean system shutdown.
		 */
		if (bdev_read_only(sb->s_bdev) &&
				!is_set_ckpt_flags(sbi, CP_UMOUNT_FLAG)) {
			err = -EROFS;
			goto free_kobj;
		}

		if (need_fsck)
			set_sbi_flag(sbi, SBI_NEED_FSCK);

		if (!retry)
			goto skip_recovery;

		err = recover_fsync_data(sbi, false);
		if (err < 0) {
			need_fsck = true;
			f2fs_msg(sb, KERN_ERR,
				"Cannot recover all fsync data errno=%d", err);
			goto free_kobj;
		}
	} else {
		err = recover_fsync_data(sbi, true);

		if (!f2fs_readonly(sb) && err > 0) {
			err = -EINVAL;
			f2fs_msg(sb, KERN_ERR,
				"Need to recover fsync data");
			goto free_kobj;
		}
	}
skip_recovery:
	/* recover_fsync_data() cleared this already */
	clear_sbi_flag(sbi, SBI_POR_DOING);

	/*
	 * If filesystem is not mounted as read-only then
	 * do start the gc_thread.
	 */
	if (test_opt(sbi, BG_GC) && !f2fs_readonly(sb)) {
		/* After POR, we can run background GC thread.*/
		err = start_gc_thread(sbi);
		if (err)
			goto free_kobj;
	}
	kfree(options);

	/* recover broken superblock */
	if (recovery) {
		err = f2fs_commit_super(sbi, true);
		f2fs_msg(sb, KERN_INFO,
			"Try to recover %dth superblock, ret: %d",
			sbi->valid_super_block ? 1 : 2, err);
	}

	f2fs_msg(sbi->sb, KERN_NOTICE, "Mounted with checkpoint version = %llx",
				cur_cp_version(F2FS_CKPT(sbi)));
	f2fs_update_time(sbi, CP_TIME);
	f2fs_update_time(sbi, REQ_TIME);
	return 0;

free_kobj:
	f2fs_sync_inode_meta(sbi);
	kobject_del(&sbi->s_kobj);
	kobject_put(&sbi->s_kobj);
	wait_for_completion(&sbi->s_kobj_unregister);
free_proc:
	if (sbi->s_proc) {
		remove_proc_entry("segment_info", sbi->s_proc);
		remove_proc_entry("segment_bits", sbi->s_proc);
		remove_proc_entry(sb->s_id, f2fs_proc_root);
	}
free_root_inode:
	dput(sb->s_root);
	sb->s_root = NULL;
free_node_inode:
	truncate_inode_pages_final(NODE_MAPPING(sbi));
	mutex_lock(&sbi->umount_mutex);
	release_ino_entry(sbi, true);
	f2fs_leave_shrinker(sbi);
	/*
	 * Some dirty meta pages can be produced by recover_orphan_inodes()
	 * failed by EIO. Then, iput(node_inode) can trigger balance_fs_bg()
	 * followed by write_checkpoint() through f2fs_write_node_pages(), which
	 * falls into an infinite loop in sync_meta_pages().
	 */
	truncate_inode_pages_final(META_MAPPING(sbi));
	iput(sbi->node_inode);
	mutex_unlock(&sbi->umount_mutex);
	f2fs_destroy_stats(sbi);
free_nm:
	destroy_node_manager(sbi);
free_sm:
	destroy_segment_manager(sbi);
free_devices:
	destroy_device_list(sbi);
	kfree(sbi->ckpt);
free_meta_inode:
	make_bad_inode(sbi->meta_inode);
	iput(sbi->meta_inode);
free_io_dummy:
	mempool_destroy(sbi->write_io_dummy);
free_options:
	destroy_percpu_info(sbi);
	kfree(options);
free_sb_buf:
	kfree(raw_super);
free_sbi:
	if (sbi->s_chksum_driver)
		crypto_free_shash(sbi->s_chksum_driver);
	kfree(sbi);

	/* give only one another chance */
	if (retry) {
		retry = false;
		shrink_dcache_sb(sb);
		goto try_onemore;
	}
	return err;
}