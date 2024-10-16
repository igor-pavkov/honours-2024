static inline unsigned int idx_hash(struct net *net, u32 index)
{
	return __idx_hash(index, net->xfrm.policy_idx_hmask);
}