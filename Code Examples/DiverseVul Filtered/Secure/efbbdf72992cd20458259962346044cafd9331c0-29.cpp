static void dns_resolve_send(struct dgram_conn *dgram)
{
	struct dns_resolvers  *resolvers;
	struct dns_nameserver *ns;
	struct dns_resolution *res;
	int fd;

	fd = dgram->t.sock.fd;

	/* check if ready for sending */
	if (!fd_send_ready(fd))
		return;

	/* we don't want/need to be waked up any more for sending */
	fd_stop_send(fd);

	/* no need to go further if we can't retrieve the nameserver */
	if ((ns = dgram->owner) == NULL)
		return;

	resolvers = ns->resolvers;
	HA_SPIN_LOCK(DNS_LOCK, &resolvers->lock);

	list_for_each_entry(res, &resolvers->resolutions.curr, list) {
		int ret, len;

		if (res->nb_queries == resolvers->nb_nameservers)
			continue;

		len = dns_build_query(res->query_id, res->query_type,
		                      resolvers->accepted_payload_size,
		                      res->hostname_dn, res->hostname_dn_len,
		                      trash.area, trash.size);
		if (len == -1)
			goto snd_error;

		ret = send(fd, trash.area, len, 0);
		if (ret != len)
			goto snd_error;

		ns->counters.sent++;
		res->nb_queries++;
		continue;

	  snd_error:
		ns->counters.snd_error++;
		res->nb_queries++;
	}
	HA_SPIN_UNLOCK(DNS_LOCK, &resolvers->lock);
}