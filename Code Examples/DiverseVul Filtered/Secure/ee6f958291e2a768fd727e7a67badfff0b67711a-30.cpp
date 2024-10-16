static void *m_start(struct seq_file *m, loff_t *pos)
{
	struct mnt_namespace *n = m->private;
	struct list_head *p;
	loff_t l = *pos;

	down_read(&namespace_sem);
	list_for_each(p, &n->list)
		if (!l--)
			return list_entry(p, struct vfsmount, mnt_list);
	return NULL;
}