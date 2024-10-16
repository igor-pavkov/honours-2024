int vmw_surface_reference_ioctl(struct drm_device *dev, void *data,
				struct drm_file *file_priv)
{
	struct vmw_private *dev_priv = vmw_priv(dev);
	union drm_vmw_surface_reference_arg *arg =
	    (union drm_vmw_surface_reference_arg *)data;
	struct drm_vmw_surface_arg *req = &arg->req;
	struct drm_vmw_surface_create_req *rep = &arg->rep;
	struct ttm_object_file *tfile = vmw_fpriv(file_priv)->tfile;
	struct vmw_surface *srf;
	struct vmw_user_surface *user_srf;
	struct drm_vmw_size __user *user_sizes;
	struct ttm_base_object *base;
	int ret;

	ret = vmw_surface_handle_reference(dev_priv, file_priv, req->sid,
					   req->handle_type, &base);
	if (unlikely(ret != 0))
		return ret;

	user_srf = container_of(base, struct vmw_user_surface, prime.base);
	srf = &user_srf->srf;

	rep->flags = srf->flags;
	rep->format = srf->format;
	memcpy(rep->mip_levels, srf->mip_levels, sizeof(srf->mip_levels));
	user_sizes = (struct drm_vmw_size __user *)(unsigned long)
	    rep->size_addr;

	if (user_sizes)
		ret = copy_to_user(user_sizes, &srf->base_size,
				   sizeof(srf->base_size));
	if (unlikely(ret != 0)) {
		DRM_ERROR("copy_to_user failed %p %u\n",
			  user_sizes, srf->num_sizes);
		ttm_ref_object_base_unref(tfile, base->hash.key, TTM_REF_USAGE);
		ret = -EFAULT;
	}

	ttm_base_object_unref(&base);

	return ret;
}