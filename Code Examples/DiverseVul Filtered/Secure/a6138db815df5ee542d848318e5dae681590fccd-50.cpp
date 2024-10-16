static void unlock_mount(struct mountpoint *where)
{
	struct dentry *dentry = where->m_dentry;
	put_mountpoint(where);
	namespace_unlock();
	mutex_unlock(&dentry->d_inode->i_mutex);
}