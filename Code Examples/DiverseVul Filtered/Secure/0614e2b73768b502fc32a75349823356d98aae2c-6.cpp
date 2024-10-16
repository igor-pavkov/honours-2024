static int ax25_info_show(struct seq_file *seq, void *v)
{
	ax25_cb *ax25 = hlist_entry(v, struct ax25_cb, ax25_node);
	char buf[11];
	int k;


	/*
	 * New format:
	 * magic dev src_addr dest_addr,digi1,digi2,.. st vs vr va t1 t1 t2 t2 t3 t3 idle idle n2 n2 rtt window paclen Snd-Q Rcv-Q inode
	 */

	seq_printf(seq, "%p %s %s%s ",
		   ax25,
		   ax25->ax25_dev == NULL? "???" : ax25->ax25_dev->dev->name,
		   ax2asc(buf, &ax25->source_addr),
		   ax25->iamdigi? "*":"");
	seq_printf(seq, "%s", ax2asc(buf, &ax25->dest_addr));

	for (k=0; (ax25->digipeat != NULL) && (k < ax25->digipeat->ndigi); k++) {
		seq_printf(seq, ",%s%s",
			   ax2asc(buf, &ax25->digipeat->calls[k]),
			   ax25->digipeat->repeated[k]? "*":"");
	}

	seq_printf(seq, " %d %d %d %d %lu %lu %lu %lu %lu %lu %lu %lu %d %d %lu %d %d",
		   ax25->state,
		   ax25->vs, ax25->vr, ax25->va,
		   ax25_display_timer(&ax25->t1timer) / HZ, ax25->t1 / HZ,
		   ax25_display_timer(&ax25->t2timer) / HZ, ax25->t2 / HZ,
		   ax25_display_timer(&ax25->t3timer) / HZ, ax25->t3 / HZ,
		   ax25_display_timer(&ax25->idletimer) / (60 * HZ),
		   ax25->idle / (60 * HZ),
		   ax25->n2count, ax25->n2,
		   ax25->rtt / HZ,
		   ax25->window,
		   ax25->paclen);

	if (ax25->sk != NULL) {
		seq_printf(seq, " %d %d %lu\n",
			   sk_wmem_alloc_get(ax25->sk),
			   sk_rmem_alloc_get(ax25->sk),
			   sock_i_ino(ax25->sk));
	} else {
		seq_puts(seq, " * * *\n");
	}
	return 0;
}