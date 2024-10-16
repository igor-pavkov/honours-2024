static int selinux_sb_remount(struct super_block *sb, void *data)
{
	int rc, i, *flags;
	struct security_mnt_opts opts;
	char *secdata, **mount_options;
	struct superblock_security_struct *sbsec = sb->s_security;

	if (!(sbsec->flags & SE_SBINITIALIZED))
		return 0;

	if (!data)
		return 0;

	if (sb->s_type->fs_flags & FS_BINARY_MOUNTDATA)
		return 0;

	security_init_mnt_opts(&opts);
	secdata = alloc_secdata();
	if (!secdata)
		return -ENOMEM;
	rc = selinux_sb_copy_data(data, secdata);
	if (rc)
		goto out_free_secdata;

	rc = selinux_parse_opts_str(secdata, &opts);
	if (rc)
		goto out_free_secdata;

	mount_options = opts.mnt_opts;
	flags = opts.mnt_opts_flags;

	for (i = 0; i < opts.num_mnt_opts; i++) {
		u32 sid;

		if (flags[i] == SBLABEL_MNT)
			continue;
		rc = security_context_str_to_sid(mount_options[i], &sid, GFP_KERNEL);
		if (rc) {
			printk(KERN_WARNING "SELinux: security_context_str_to_sid"
			       "(%s) failed for (dev %s, type %s) errno=%d\n",
			       mount_options[i], sb->s_id, sb->s_type->name, rc);
			goto out_free_opts;
		}
		rc = -EINVAL;
		switch (flags[i]) {
		case FSCONTEXT_MNT:
			if (bad_option(sbsec, FSCONTEXT_MNT, sbsec->sid, sid))
				goto out_bad_option;
			break;
		case CONTEXT_MNT:
			if (bad_option(sbsec, CONTEXT_MNT, sbsec->mntpoint_sid, sid))
				goto out_bad_option;
			break;
		case ROOTCONTEXT_MNT: {
			struct inode_security_struct *root_isec;
			root_isec = backing_inode_security(sb->s_root);

			if (bad_option(sbsec, ROOTCONTEXT_MNT, root_isec->sid, sid))
				goto out_bad_option;
			break;
		}
		case DEFCONTEXT_MNT:
			if (bad_option(sbsec, DEFCONTEXT_MNT, sbsec->def_sid, sid))
				goto out_bad_option;
			break;
		default:
			goto out_free_opts;
		}
	}

	rc = 0;
out_free_opts:
	security_free_mnt_opts(&opts);
out_free_secdata:
	free_secdata(secdata);
	return rc;
out_bad_option:
	printk(KERN_WARNING "SELinux: unable to change security options "
	       "during remount (dev %s, type=%s)\n", sb->s_id,
	       sb->s_type->name);
	goto out_free_opts;
}