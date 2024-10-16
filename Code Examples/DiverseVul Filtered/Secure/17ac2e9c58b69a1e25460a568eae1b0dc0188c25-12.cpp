static int rose_info_show(struct seq_file *seq, void *v)
{
	char buf[11], rsbuf[11];

	if (v == SEQ_START_TOKEN)
		seq_puts(seq,
			 "dest_addr  dest_call src_addr   src_call  dev   lci neigh st vs vr va   t  t1  t2  t3  hb    idle Snd-Q Rcv-Q inode\n");

	else {
		struct sock *s = v;
		struct rose_sock *rose = rose_sk(s);
		const char *devname, *callsign;
		const struct net_device *dev = rose->device;

		if (!dev)
			devname = "???";
		else
			devname = dev->name;

		seq_printf(seq, "%-10s %-9s ",
			   rose2asc(rsbuf, &rose->dest_addr),
			   ax2asc(buf, &rose->dest_call));

		if (ax25cmp(&rose->source_call, &null_ax25_address) == 0)
			callsign = "??????-?";
		else
			callsign = ax2asc(buf, &rose->source_call);

		seq_printf(seq,
			   "%-10s %-9s %-5s %3.3X %05d  %d  %d  %d  %d %3lu %3lu %3lu %3lu %3lu %3lu/%03lu %5d %5d %ld\n",
			rose2asc(rsbuf, &rose->source_addr),
			callsign,
			devname,
			rose->lci & 0x0FFF,
			(rose->neighbour) ? rose->neighbour->number : 0,
			rose->state,
			rose->vs,
			rose->vr,
			rose->va,
			ax25_display_timer(&rose->timer) / HZ,
			rose->t1 / HZ,
			rose->t2 / HZ,
			rose->t3 / HZ,
			rose->hb / HZ,
			ax25_display_timer(&rose->idletimer) / (60 * HZ),
			rose->idle / (60 * HZ),
			sk_wmem_alloc_get(s),
			sk_rmem_alloc_get(s),
			s->sk_socket ? SOCK_INODE(s->sk_socket)->i_ino : 0L);
	}

	return 0;
}