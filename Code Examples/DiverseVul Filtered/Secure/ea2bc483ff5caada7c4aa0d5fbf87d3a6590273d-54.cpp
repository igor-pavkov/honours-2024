static int sctp_getsockopt_local_addrs_num_old(struct sock *sk, int len,
					       char __user *optval,
					       int __user *optlen)
{
	sctp_assoc_t id;
	struct sctp_bind_addr *bp;
	struct sctp_association *asoc;
	struct list_head *pos, *temp;
	struct sctp_sockaddr_entry *addr;
	rwlock_t *addr_lock;
	int cnt = 0;

	if (len != sizeof(sctp_assoc_t))
		return -EINVAL;

	if (copy_from_user(&id, optval, sizeof(sctp_assoc_t)))
		return -EFAULT;

	/*
	 *  For UDP-style sockets, id specifies the association to query.
	 *  If the id field is set to the value '0' then the locally bound
	 *  addresses are returned without regard to any particular
	 *  association.
	 */
	if (0 == id) {
		bp = &sctp_sk(sk)->ep->base.bind_addr;
		addr_lock = &sctp_sk(sk)->ep->base.addr_lock;
	} else {
		asoc = sctp_id2assoc(sk, id);
		if (!asoc)
			return -EINVAL;
		bp = &asoc->base.bind_addr;
		addr_lock = &asoc->base.addr_lock;
	}

	sctp_read_lock(addr_lock);

	/* If the endpoint is bound to 0.0.0.0 or ::0, count the valid
	 * addresses from the global local address list.
	 */
	if (sctp_list_single_entry(&bp->address_list)) {
		addr = list_entry(bp->address_list.next,
				  struct sctp_sockaddr_entry, list);
		if (sctp_is_any(&addr->a)) {
			list_for_each_safe(pos, temp, &sctp_local_addr_list) {
				addr = list_entry(pos,
						  struct sctp_sockaddr_entry,
						  list);
				if ((PF_INET == sk->sk_family) &&
				    (AF_INET6 == addr->a.sa.sa_family))
					continue;
				cnt++;
			}
		} else {
			cnt = 1;
		}
		goto done;
	}

	list_for_each(pos, &bp->address_list) {
		cnt ++;
	}

done:
	sctp_read_unlock(addr_lock);
	return cnt;
}