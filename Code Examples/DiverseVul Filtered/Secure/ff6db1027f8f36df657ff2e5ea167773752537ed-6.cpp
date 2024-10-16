void bgp_notify_send(struct peer *peer, uint8_t code, uint8_t sub_code)
{
	bgp_notify_send_with_data(peer, code, sub_code, NULL, 0);
}