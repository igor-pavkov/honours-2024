static void snd_usbmidi_raw_output(struct snd_usb_midi_out_endpoint *ep,
				   struct urb *urb)
{
	int count;

	if (!ep->ports[0].active)
		return;
	count = snd_rawmidi_transmit(ep->ports[0].substream,
				     urb->transfer_buffer,
				     ep->max_transfer);
	if (count < 1) {
		ep->ports[0].active = 0;
		return;
	}
	urb->transfer_buffer_length = count;
}