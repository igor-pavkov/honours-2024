static int selinux_nlmsg_perm(struct sock *sk, struct sk_buff *skb)
{
	int err = 0;
	u32 perm;
	struct nlmsghdr *nlh;
	struct sk_security_struct *sksec = sk->sk_security;

	if (skb->len < NLMSG_HDRLEN) {
		err = -EINVAL;
		goto out;
	}
	nlh = nlmsg_hdr(skb);

	err = selinux_nlmsg_lookup(sksec->sclass, nlh->nlmsg_type, &perm);
	if (err) {
		if (err == -EINVAL) {
			pr_warn_ratelimited("SELinux: unrecognized netlink"
			       " message: protocol=%hu nlmsg_type=%hu sclass=%s"
			       " pig=%d comm=%s\n",
			       sk->sk_protocol, nlh->nlmsg_type,
			       secclass_map[sksec->sclass - 1].name,
			       task_pid_nr(current), current->comm);
			if (!selinux_enforcing || security_get_allow_unknown())
				err = 0;
		}

		/* Ignore */
		if (err == -ENOENT)
			err = 0;
		goto out;
	}

	err = sock_has_perm(current, sk, perm);
out:
	return err;
}