int nft_expr_clone(struct nft_expr *dst, struct nft_expr *src)
{
	int err;

	if (src->ops->clone) {
		dst->ops = src->ops;
		err = src->ops->clone(dst, src);
		if (err < 0)
			return err;
	} else {
		memcpy(dst, src, src->ops->size);
	}

	__module_get(src->ops->type->owner);

	return 0;
}