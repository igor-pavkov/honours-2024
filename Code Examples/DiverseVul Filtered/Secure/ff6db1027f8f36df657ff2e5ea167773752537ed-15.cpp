int bgp_packet_set_marker(struct stream *s, uint8_t type)
{
	int i;

	/* Fill in marker. */
	for (i = 0; i < BGP_MARKER_SIZE; i++)
		stream_putc(s, 0xff);

	/* Dummy total length. This field is should be filled in later on. */
	stream_putw(s, 0);

	/* BGP packet type. */
	stream_putc(s, type);

	/* Return current stream size. */
	return stream_get_endp(s);
}