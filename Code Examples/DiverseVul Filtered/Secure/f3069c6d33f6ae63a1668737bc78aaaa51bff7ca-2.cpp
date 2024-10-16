int rds_get_mr_for_dest(struct rds_sock *rs, char __user *optval, int optlen)
{
	struct rds_get_mr_for_dest_args args;
	struct rds_get_mr_args new_args;

	if (optlen != sizeof(struct rds_get_mr_for_dest_args))
		return -EINVAL;

	if (copy_from_user(&args, (struct rds_get_mr_for_dest_args __user *)optval,
			   sizeof(struct rds_get_mr_for_dest_args)))
		return -EFAULT;

	/*
	 * Initially, just behave like get_mr().
	 * TODO: Implement get_mr as wrapper around this
	 *	 and deprecate it.
	 */
	new_args.vec = args.vec;
	new_args.cookie_addr = args.cookie_addr;
	new_args.flags = args.flags;

	return __rds_rdma_map(rs, &new_args, NULL, NULL);
}