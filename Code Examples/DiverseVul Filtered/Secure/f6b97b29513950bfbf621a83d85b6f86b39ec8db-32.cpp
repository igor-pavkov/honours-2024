static void nr_info_stop(struct seq_file *seq, void *v)
{
	spin_unlock_bh(&nr_list_lock);
}