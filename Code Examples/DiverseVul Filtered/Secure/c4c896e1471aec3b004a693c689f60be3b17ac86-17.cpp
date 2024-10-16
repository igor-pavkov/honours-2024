static inline struct sock *sco_chan_get(struct sco_conn *conn)
{
	struct sock *sk = NULL;
	sco_conn_lock(conn);
	sk = conn->sk;
	sco_conn_unlock(conn);
	return sk;
}