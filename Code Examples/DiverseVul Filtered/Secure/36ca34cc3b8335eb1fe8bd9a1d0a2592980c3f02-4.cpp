static int ipip6_tunnel_get_prl(struct ip_tunnel *t, struct ip_tunnel_prl *a)
{
	struct ip_tunnel_prl *kp;
	struct ip_tunnel_prl_entry *prl;
	unsigned int cmax, c = 0, ca, len;
	int ret = 0;

	cmax = a->datalen / sizeof(*a);
	if (cmax > 1 && a->addr != htonl(INADDR_ANY))
		cmax = 1;

	/* For simple GET or for root users,
	 * we try harder to allocate.
	 */
	kp = (cmax <= 1 || capable(CAP_NET_ADMIN)) ?
		kcalloc(cmax, sizeof(*kp), GFP_KERNEL) :
		NULL;

	read_lock(&ipip6_lock);

	ca = t->prl_count < cmax ? t->prl_count : cmax;

	if (!kp) {
		/* We don't try hard to allocate much memory for
		 * non-root users.
		 * For root users, retry allocating enough memory for
		 * the answer.
		 */
		kp = kcalloc(ca, sizeof(*kp), GFP_ATOMIC);
		if (!kp) {
			ret = -ENOMEM;
			goto out;
		}
	}

	c = 0;
	for (prl = t->prl; prl; prl = prl->next) {
		if (c > cmax)
			break;
		if (a->addr != htonl(INADDR_ANY) && prl->addr != a->addr)
			continue;
		kp[c].addr = prl->addr;
		kp[c].flags = prl->flags;
		c++;
		if (a->addr != htonl(INADDR_ANY))
			break;
	}
out:
	read_unlock(&ipip6_lock);

	len = sizeof(*kp) * c;
	ret = len ? copy_to_user(a->data, kp, len) : 0;

	kfree(kp);
	if (ret)
		return -EFAULT;

	a->datalen = len;
	return 0;
}