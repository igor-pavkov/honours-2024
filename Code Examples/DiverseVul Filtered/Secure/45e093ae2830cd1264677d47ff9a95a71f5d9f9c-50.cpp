static int tipc_sendmcast(struct  socket *sock, struct tipc_name_seq *seq,
			  struct msghdr *msg, size_t dsz, long timeo)
{
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	struct net *net = sock_net(sk);
	struct tipc_msg *mhdr = &tsk->phdr;
	struct sk_buff_head pktchain;
	struct iov_iter save = msg->msg_iter;
	uint mtu;
	int rc;

	msg_set_type(mhdr, TIPC_MCAST_MSG);
	msg_set_lookup_scope(mhdr, TIPC_CLUSTER_SCOPE);
	msg_set_destport(mhdr, 0);
	msg_set_destnode(mhdr, 0);
	msg_set_nametype(mhdr, seq->type);
	msg_set_namelower(mhdr, seq->lower);
	msg_set_nameupper(mhdr, seq->upper);
	msg_set_hdr_sz(mhdr, MCAST_H_SIZE);

	skb_queue_head_init(&pktchain);

new_mtu:
	mtu = tipc_bcast_get_mtu(net);
	rc = tipc_msg_build(mhdr, msg, 0, dsz, mtu, &pktchain);
	if (unlikely(rc < 0))
		return rc;

	do {
		rc = tipc_bcast_xmit(net, &pktchain);
		if (likely(!rc))
			return dsz;

		if (rc == -ELINKCONG) {
			tsk->link_cong = 1;
			rc = tipc_wait_for_sndmsg(sock, &timeo);
			if (!rc)
				continue;
		}
		__skb_queue_purge(&pktchain);
		if (rc == -EMSGSIZE) {
			msg->msg_iter = save;
			goto new_mtu;
		}
		break;
	} while (1);
	return rc;
}