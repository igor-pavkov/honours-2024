static int hidp_send_intr_message(struct hidp_session *session,
				  unsigned char hdr, const unsigned char *data,
				  int size)
{
	return hidp_send_message(session, session->intr_sock,
				 &session->intr_transmit, hdr, data, size);
}