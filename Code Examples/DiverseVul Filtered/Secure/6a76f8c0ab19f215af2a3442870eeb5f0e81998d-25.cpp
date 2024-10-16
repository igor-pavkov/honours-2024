static void g_stop(struct seq_file *m, void *p)
{
	mutex_unlock(&graph_lock);
}