struct dst_entry *__sk_dst_check(struct sock *sk, u32 cookie)
{
	struct dst_entry *dst = sk->sk_dst_cache;

	if (dst && dst->obsolete && dst->ops->check(dst, cookie) == NULL) {
		sk->sk_dst_cache = NULL;
		dst_release(dst);
		return NULL;
	}

	return dst;
}