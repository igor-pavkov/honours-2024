void snd_usbmidi_disconnect(struct list_head *p)
{
	struct snd_usb_midi *umidi;
	unsigned int i, j;

	umidi = list_entry(p, struct snd_usb_midi, list);
	/*
	 * an URB's completion handler may start the timer and
	 * a timer may submit an URB. To reliably break the cycle
	 * a flag under lock must be used
	 */
	down_write(&umidi->disc_rwsem);
	spin_lock_irq(&umidi->disc_lock);
	umidi->disconnected = 1;
	spin_unlock_irq(&umidi->disc_lock);
	up_write(&umidi->disc_rwsem);

	for (i = 0; i < MIDI_MAX_ENDPOINTS; ++i) {
		struct snd_usb_midi_endpoint *ep = &umidi->endpoints[i];
		if (ep->out)
			tasklet_kill(&ep->out->tasklet);
		if (ep->out) {
			for (j = 0; j < OUTPUT_URBS; ++j)
				usb_kill_urb(ep->out->urbs[j].urb);
			if (umidi->usb_protocol_ops->finish_out_endpoint)
				umidi->usb_protocol_ops->finish_out_endpoint(ep->out);
			ep->out->active_urbs = 0;
			if (ep->out->drain_urbs) {
				ep->out->drain_urbs = 0;
				wake_up(&ep->out->drain_wait);
			}
		}
		if (ep->in)
			for (j = 0; j < INPUT_URBS; ++j)
				usb_kill_urb(ep->in->urbs[j]);
		/* free endpoints here; later call can result in Oops */
		if (ep->out)
			snd_usbmidi_out_endpoint_clear(ep->out);
		if (ep->in) {
			snd_usbmidi_in_endpoint_delete(ep->in);
			ep->in = NULL;
		}
	}
	del_timer_sync(&umidi->error_timer);
}