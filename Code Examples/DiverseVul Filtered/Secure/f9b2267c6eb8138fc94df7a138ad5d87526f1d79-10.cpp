static NTSTATUS ldapsrv_packet_check(
	void *private_data,
	DATA_BLOB blob,
	size_t *packet_size)
{
	NTSTATUS ret;
	struct ldapsrv_connection *conn = private_data;
	int result = LDB_SUCCESS;

	ret = ldap_full_packet(private_data, blob, packet_size);
	if (!NT_STATUS_IS_OK(ret)) {
		return ret;
	}
	result = ldapsrv_check_packet_size(conn, *packet_size);
	if (result != LDAP_SUCCESS) {
		return NT_STATUS_LDAP(result);
	}
	return NT_STATUS_OK;
}