static struct sk_buff *tipc_get_err_tlv(char *str)
{
	int str_len = strlen(str) + 1;
	struct sk_buff *buf;

	buf = tipc_tlv_alloc(TLV_SPACE(str_len));
	if (buf)
		tipc_add_tlv(buf, TIPC_TLV_ERROR_STRING, str, str_len);

	return buf;
}