static int verify_one_alg(struct nlattr **attrs, enum xfrm_attr_type_t type)
{
	struct nlattr *rt = attrs[type];
	struct xfrm_algo *algp;

	if (!rt)
		return 0;

	algp = nla_data(rt);
	if (nla_len(rt) < (int)xfrm_alg_len(algp))
		return -EINVAL;

	switch (type) {
	case XFRMA_ALG_AUTH:
	case XFRMA_ALG_CRYPT:
	case XFRMA_ALG_COMP:
		break;

	default:
		return -EINVAL;
	}

	algp->alg_name[sizeof(algp->alg_name) - 1] = '\0';
	return 0;
}