static int sctp_getsockopt_peeloff(struct sock *sk, int len, char __user *optval, int __user *optlen)
{
	sctp_peeloff_arg_t peeloff;
	struct socket *newsock;
	int retval = 0;
	struct sctp_association *asoc;

	if (len != sizeof(sctp_peeloff_arg_t))
		return -EINVAL;
	if (copy_from_user(&peeloff, optval, len))
		return -EFAULT;

	asoc = sctp_id2assoc(sk, peeloff.associd);
	if (!asoc) {
		retval = -EINVAL;
		goto out;
	}

	SCTP_DEBUG_PRINTK("%s: sk: %p asoc: %p\n", __FUNCTION__, sk, asoc);

	retval = sctp_do_peeloff(asoc, &newsock);
	if (retval < 0)
		goto out;

	/* Map the socket to an unused fd that can be returned to the user.  */
	retval = sock_map_fd(newsock);
	if (retval < 0) {
		sock_release(newsock);
		goto out;
	}

	SCTP_DEBUG_PRINTK("%s: sk: %p asoc: %p newsk: %p sd: %d\n",
			  __FUNCTION__, sk, asoc, newsock->sk, retval);

	/* Return the fd mapped to the new socket.  */
	peeloff.sd = retval;
	if (copy_to_user(optval, &peeloff, len))
		retval = -EFAULT;

out:
	return retval;
}