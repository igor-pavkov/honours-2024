void xfrm_policy_walk_done(struct xfrm_policy_walk *walk, struct net *net)
{
	if (list_empty(&walk->walk.all))
		return;

	spin_lock_bh(&net->xfrm.xfrm_policy_lock); /*FIXME where is net? */
	list_del(&walk->walk.all);
	spin_unlock_bh(&net->xfrm.xfrm_policy_lock);
}