i915_gem_execbuffer_move_to_gpu(struct intel_ring_buffer *ring,
				struct list_head *objects)
{
	struct drm_i915_gem_object *obj;
	struct change_domains cd;
	int ret;

	memset(&cd, 0, sizeof(cd));
	list_for_each_entry(obj, objects, exec_list)
		i915_gem_object_set_to_gpu_domain(obj, ring, &cd);

	if (cd.invalidate_domains | cd.flush_domains) {
		ret = i915_gem_execbuffer_flush(ring->dev,
						cd.invalidate_domains,
						cd.flush_domains,
						cd.flush_rings);
		if (ret)
			return ret;
	}

	if (cd.flips) {
		ret = i915_gem_execbuffer_wait_for_flips(ring, cd.flips);
		if (ret)
			return ret;
	}

	list_for_each_entry(obj, objects, exec_list) {
		ret = i915_gem_execbuffer_sync_rings(obj, ring);
		if (ret)
			return ret;
	}

	return 0;
}