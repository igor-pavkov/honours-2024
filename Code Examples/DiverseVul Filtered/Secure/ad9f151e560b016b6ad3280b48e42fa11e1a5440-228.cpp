static struct nft_object *nft_obj_lookup_byhandle(const struct nft_table *table,
						  const struct nlattr *nla,
						  u32 objtype, u8 genmask)
{
	struct nft_object *obj;

	list_for_each_entry(obj, &table->objects, list) {
		if (be64_to_cpu(nla_get_be64(nla)) == obj->handle &&
		    objtype == obj->ops->type->type &&
		    nft_active_genmask(obj, genmask))
			return obj;
	}
	return ERR_PTR(-ENOENT);
}