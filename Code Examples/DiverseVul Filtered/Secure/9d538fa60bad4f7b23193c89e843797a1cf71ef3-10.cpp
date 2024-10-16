ssize_t sock_no_sendpage_locked(struct sock *sk, struct page *page,
				int offset, size_t size, int flags)
{
	ssize_t res;
	struct msghdr msg = {.msg_flags = flags};
	struct kvec iov;
	char *kaddr = kmap(page);

	iov.iov_base = kaddr + offset;
	iov.iov_len = size;
	res = kernel_sendmsg_locked(sk, &msg, &iov, 1, size);
	kunmap(page);
	return res;
}