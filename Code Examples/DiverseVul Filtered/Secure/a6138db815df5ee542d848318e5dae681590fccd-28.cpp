static inline void namespace_lock(void)
{
	down_write(&namespace_sem);
}