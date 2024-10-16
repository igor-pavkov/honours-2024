static void vmw_user_surface_base_release(struct ttm_base_object **p_base)
{
	struct ttm_base_object *base = *p_base;
	struct vmw_user_surface *user_srf =
	    container_of(base, struct vmw_user_surface, prime.base);
	struct vmw_resource *res = &user_srf->srf.res;

	*p_base = NULL;
	if (user_srf->backup_base)
		ttm_base_object_unref(&user_srf->backup_base);
	vmw_resource_unreference(&res);
}