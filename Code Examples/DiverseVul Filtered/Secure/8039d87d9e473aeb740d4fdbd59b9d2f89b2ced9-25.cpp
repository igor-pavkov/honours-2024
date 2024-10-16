static long btrfs_ioctl_qgroup_assign(struct file *file, void __user *arg)
{
	struct btrfs_root *root = BTRFS_I(file_inode(file))->root;
	struct btrfs_ioctl_qgroup_assign_args *sa;
	struct btrfs_trans_handle *trans;
	int ret;
	int err;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	ret = mnt_want_write_file(file);
	if (ret)
		return ret;

	sa = memdup_user(arg, sizeof(*sa));
	if (IS_ERR(sa)) {
		ret = PTR_ERR(sa);
		goto drop_write;
	}

	trans = btrfs_join_transaction(root);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		goto out;
	}

	/* FIXME: check if the IDs really exist */
	if (sa->assign) {
		ret = btrfs_add_qgroup_relation(trans, root->fs_info,
						sa->src, sa->dst);
	} else {
		ret = btrfs_del_qgroup_relation(trans, root->fs_info,
						sa->src, sa->dst);
	}

	/* update qgroup status and info */
	err = btrfs_run_qgroups(trans, root->fs_info);
	if (err < 0)
		btrfs_std_error(root->fs_info, ret,
			    "failed to update qgroup status and info\n");
	err = btrfs_end_transaction(trans, root);
	if (err && !ret)
		ret = err;

out:
	kfree(sa);
drop_write:
	mnt_drop_write_file(file);
	return ret;
}