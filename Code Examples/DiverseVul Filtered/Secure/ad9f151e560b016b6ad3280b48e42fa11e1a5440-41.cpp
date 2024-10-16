static struct nft_object *nft_obj_init(const struct nft_ctx *ctx,
				       const struct nft_object_type *type,
				       const struct nlattr *attr)
{
	struct nlattr **tb;
	const struct nft_object_ops *ops;
	struct nft_object *obj;
	int err = -ENOMEM;

	tb = kmalloc_array(type->maxattr + 1, sizeof(*tb), GFP_KERNEL);
	if (!tb)
		goto err1;

	if (attr) {
		err = nla_parse_nested_deprecated(tb, type->maxattr, attr,
						  type->policy, NULL);
		if (err < 0)
			goto err2;
	} else {
		memset(tb, 0, sizeof(tb[0]) * (type->maxattr + 1));
	}

	if (type->select_ops) {
		ops = type->select_ops(ctx, (const struct nlattr * const *)tb);
		if (IS_ERR(ops)) {
			err = PTR_ERR(ops);
			goto err2;
		}
	} else {
		ops = type->ops;
	}

	err = -ENOMEM;
	obj = kzalloc(sizeof(*obj) + ops->size, GFP_KERNEL);
	if (!obj)
		goto err2;

	err = ops->init(ctx, (const struct nlattr * const *)tb, obj);
	if (err < 0)
		goto err3;

	obj->ops = ops;

	kfree(tb);
	return obj;
err3:
	kfree(obj);
err2:
	kfree(tb);
err1:
	return ERR_PTR(err);
}