static int nr_info_show(struct seq_file *seq, void *v)
{
	struct sock *s = v;
	struct net_device *dev;
	struct nr_sock *nr;
	const char *devname;
	char buf[11];

	if (v == SEQ_START_TOKEN)
		seq_puts(seq,
"user_addr dest_node src_node  dev    my  your  st  vs  vr  va    t1     t2     t4      idle   n2  wnd Snd-Q Rcv-Q inode\n");

	else {

		bh_lock_sock(s);
		nr = nr_sk(s);

		if ((dev = nr->device) == NULL)
			devname = "???";
		else
			devname = dev->name;

		seq_printf(seq, "%-9s ", ax2asc(buf, &nr->user_addr));
		seq_printf(seq, "%-9s ", ax2asc(buf, &nr->dest_addr));
		seq_printf(seq,
"%-9s %-3s  %02X/%02X %02X/%02X %2d %3d %3d %3d %3lu/%03lu %2lu/%02lu %3lu/%03lu %3lu/%03lu %2d/%02d %3d %5d %5d %ld\n",
			ax2asc(buf, &nr->source_addr),
			devname,
			nr->my_index,
			nr->my_id,
			nr->your_index,
			nr->your_id,
			nr->state,
			nr->vs,
			nr->vr,
			nr->va,
			ax25_display_timer(&nr->t1timer) / HZ,
			nr->t1 / HZ,
			ax25_display_timer(&nr->t2timer) / HZ,
			nr->t2 / HZ,
			ax25_display_timer(&nr->t4timer) / HZ,
			nr->t4 / HZ,
			ax25_display_timer(&nr->idletimer) / (60 * HZ),
			nr->idle / (60 * HZ),
			nr->n2count,
			nr->n2,
			nr->window,
			sk_wmem_alloc_get(s),
			sk_rmem_alloc_get(s),
			s->sk_socket ? SOCK_INODE(s->sk_socket)->i_ino : 0L);

		bh_unlock_sock(s);
	}
	return 0;
}