static int selinux_conn_sid(u32 sk_sid, u32 skb_sid, u32 *conn_sid)
{
	int err = 0;

	if (skb_sid != SECSID_NULL)
		err = security_sid_mls_copy(sk_sid, skb_sid, conn_sid);
	else
		*conn_sid = sk_sid;

	return err;
}