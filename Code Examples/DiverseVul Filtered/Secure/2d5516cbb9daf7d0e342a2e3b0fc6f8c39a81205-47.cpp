static int unshare_fs(unsigned long unshare_flags, struct fs_struct **new_fsp)
{
	struct fs_struct *fs = current->fs;

	if ((unshare_flags & CLONE_FS) &&
	    (fs && atomic_read(&fs->count) > 1)) {
		*new_fsp = __copy_fs_struct(current->fs);
		if (!*new_fsp)
			return -ENOMEM;
	}

	return 0;
}