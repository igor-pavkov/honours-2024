static int hidp_send_ctrl_message(struct hidp_session *session,
				  unsigned char hdr, const unsigned char *data,
				  int size)
{
	return hidp_send_message(session, session->ctrl_sock,
				 &session->ctrl_transmit, hdr, data, size);
}