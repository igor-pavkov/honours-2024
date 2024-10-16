static int ext4dev_get_sb(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *data, struct vfsmount *mnt)
{
	printk(KERN_WARNING "EXT4-fs: Update your userspace programs "
	       "to mount using ext4\n");
	printk(KERN_WARNING "EXT4-fs: ext4dev backwards compatibility "
	       "will go away by 2.6.31\n");
	return get_sb_bdev(fs_type, flags, dev_name, data, ext4_fill_super, mnt);
}