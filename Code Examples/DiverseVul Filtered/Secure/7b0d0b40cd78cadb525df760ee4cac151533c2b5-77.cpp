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
			audit_log(current->audit_context, GFP_KERNEL, AUDIT_SELINUX_ERR,
				  "SELinux:  unrecognized netlink message"
				  " type=%hu for sclass=%hu\n",
				  nlh->nlmsg_type, sksec->sclass);
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