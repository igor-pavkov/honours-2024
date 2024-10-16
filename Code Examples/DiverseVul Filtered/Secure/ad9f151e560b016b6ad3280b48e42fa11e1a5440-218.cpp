static void lockdep_nfnl_nft_mutex_not_held(void)
{
#ifdef CONFIG_PROVE_LOCKING
	if (debug_locks)
		WARN_ON_ONCE(lockdep_nfnl_is_held(NFNL_SUBSYS_NFTABLES));
#endif
}