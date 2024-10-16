static gboolean session_abort_pending_pdu(struct avrcp *session)
{
	if (session->pending_pdu == NULL)
		return FALSE;

	g_list_free(session->pending_pdu->attr_ids);
	g_free(session->pending_pdu);
	session->pending_pdu = NULL;

	return TRUE;
}