void bgp_packet_set_size(struct stream *s)
{
	int cp;

	/* Preserve current pointer. */
	cp = stream_get_endp(s);
	stream_putw_at(s, BGP_MARKER_SIZE, cp);
}