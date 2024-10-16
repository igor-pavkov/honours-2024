static inline void vmx_segment_cache_clear(struct vcpu_vmx *vmx)
{
	vmx->segment_cache.bitmask = 0;
}