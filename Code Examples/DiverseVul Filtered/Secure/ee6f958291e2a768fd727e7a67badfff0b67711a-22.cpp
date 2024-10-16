static void *m_next(struct seq_file *m, void *v, loff_t *pos)
{
	struct mnt_namespace *n = m->private;
	struct list_head *p = ((struct vfsmount *)v)->mnt_list.next;
	(*pos)++;
	return p == &n->list ? NULL : list_entry(p, struct vfsmount, mnt_list);
}