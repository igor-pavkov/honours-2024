static inline void free_signal_struct(struct signal_struct *sig)
{
	taskstats_tgid_free(sig);
	kmem_cache_free(signal_cachep, sig);
}