static inline void clear_timeout(struct aio_timeout *to)
{
	del_singleshot_timer_sync(&to->timer);
}