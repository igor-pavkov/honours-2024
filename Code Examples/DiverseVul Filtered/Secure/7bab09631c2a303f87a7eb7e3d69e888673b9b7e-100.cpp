void xfrm_policy_walk_init(struct xfrm_policy_walk *walk, u8 type)
{
	INIT_LIST_HEAD(&walk->walk.all);
	walk->walk.dead = 1;
	walk->type = type;
	walk->seq = 0;
}