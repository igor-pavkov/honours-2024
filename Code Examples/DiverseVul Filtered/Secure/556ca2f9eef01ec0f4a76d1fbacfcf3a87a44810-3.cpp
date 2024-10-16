RZ_API ut64 rz_dyldcache_va2pa(RzDyldCache *cache, uint64_t vaddr, ut32 *offset, ut32 *left) {
	rz_return_val_if_fail(cache, UT64_MAX);
	ut64 slide = rz_dyldcache_get_slide(cache);
	ut64 res = va2pa(vaddr, cache->n_maps, cache->maps, cache->buf, slide, offset, left);
	if (res == UT64_MAX) {
		res = 0;
	}
	return res;
}