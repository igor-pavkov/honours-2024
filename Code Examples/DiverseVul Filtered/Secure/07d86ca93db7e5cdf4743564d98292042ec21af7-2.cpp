static void update_roland_altsetting(struct snd_usb_midi *umidi)
{
	struct usb_interface *intf;
	struct usb_host_interface *hostif;
	struct usb_interface_descriptor *intfd;
	int is_light_load;

	intf = umidi->iface;
	is_light_load = intf->cur_altsetting != intf->altsetting;
	if (umidi->roland_load_ctl->private_value == is_light_load)
		return;
	hostif = &intf->altsetting[umidi->roland_load_ctl->private_value];
	intfd = get_iface_desc(hostif);
	snd_usbmidi_input_stop(&umidi->list);
	usb_set_interface(umidi->dev, intfd->bInterfaceNumber,
			  intfd->bAlternateSetting);
	snd_usbmidi_input_start(&umidi->list);
}