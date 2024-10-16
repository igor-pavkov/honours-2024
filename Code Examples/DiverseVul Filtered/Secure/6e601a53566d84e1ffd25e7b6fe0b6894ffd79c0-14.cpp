static inline void sock_diag_unlock_handler(const struct sock_diag_handler *h)
{
	mutex_unlock(&sock_diag_table_mutex);
}