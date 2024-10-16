static int attach_crypt(struct xfrm_state *x, struct nlattr *rta)
{
	struct xfrm_algo *p, *ualg;
	struct xfrm_algo_desc *algo;

	if (!rta)
		return 0;

	ualg = nla_data(rta);

	algo = xfrm_ealg_get_byname(ualg->alg_name, 1);
	if (!algo)
		return -ENOSYS;
	x->props.ealgo = algo->desc.sadb_alg_id;

	p = kmemdup(ualg, xfrm_alg_len(ualg), GFP_KERNEL);
	if (!p)
		return -ENOMEM;

	strcpy(p->alg_name, algo->name);
	x->ealg = p;
	x->geniv = algo->uinfo.encr.geniv;
	return 0;
}