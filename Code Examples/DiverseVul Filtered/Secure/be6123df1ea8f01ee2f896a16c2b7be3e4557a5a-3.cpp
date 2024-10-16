static void setup_ret_submit_pdu(struct usbip_header *rpdu, struct urb *urb)
{
	struct stub_priv *priv = (struct stub_priv *) urb->context;

	setup_base_pdu(&rpdu->base, USBIP_RET_SUBMIT, priv->seqnum);
	usbip_pack_pdu(rpdu, urb, USBIP_RET_SUBMIT, 1);
}