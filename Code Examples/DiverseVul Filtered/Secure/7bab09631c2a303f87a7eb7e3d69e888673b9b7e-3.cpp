static void xfrm_hash_resize(struct work_struct *work)
{
	struct net *net = container_of(work, struct net, xfrm.policy_hash_work);
	int dir, total;

	mutex_lock(&hash_resize_mutex);

	total = 0;
	for (dir = 0; dir < XFRM_POLICY_MAX; dir++) {
		if (xfrm_bydst_should_resize(net, dir, &total))
			xfrm_bydst_resize(net, dir);
	}
	if (xfrm_byidx_should_resize(net, total))
		xfrm_byidx_resize(net, total);

	mutex_unlock(&hash_resize_mutex);
}