static void rebase_buffer(RKernelCacheObj *obj, ut64 off, RIODesc *fd, ut8 *buf, int count) {
	if (obj->rebasing_buffer || !buf) {
		return;
	}
	obj->rebasing_buffer = true;

	ut64 eob = off + count;
	int i = 0;
	RRebaseCtx ctx;

	ctx.off = off;
	ctx.eob = eob;
	ctx.buf = buf;
	ctx.count = count;
	ctx.obj = obj;

	for (; i < obj->rebase_info->n_ranges; i++) {
		ut64 start = obj->rebase_info->ranges[i].offset;
		ut64 end = start + obj->rebase_info->ranges[i].size;
		if (end >= off && start <= eob) {
			iterate_rebase_list (obj->cache_buf, obj->rebase_info->multiplier, start,
				(ROnRebaseFunc) on_rebase_pointer, &ctx);
		}
	}

	obj->rebasing_buffer = false;
}