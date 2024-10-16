static void *m_next(struct seq_file *m, void *v, loff_t *pos)
{
	struct proc_mounts *p = proc_mounts(m);

	p->cached_mount = seq_list_next(v, &p->ns->list, pos);
	p->cached_index = *pos;
	return p->cached_mount;
}