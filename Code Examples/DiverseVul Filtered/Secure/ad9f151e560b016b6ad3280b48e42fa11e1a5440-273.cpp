static void nft_obj_destroy(const struct nft_ctx *ctx, struct nft_object *obj)
{
	if (obj->ops->destroy)
		obj->ops->destroy(ctx, obj);

	module_put(obj->ops->type->owner);
	kfree(obj->key.name);
	kfree(obj->udata);
	kfree(obj);
}