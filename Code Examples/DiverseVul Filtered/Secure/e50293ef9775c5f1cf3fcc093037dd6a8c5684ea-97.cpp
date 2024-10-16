static void set_port_led(struct usb_hub *hub, int port1, int selector)
{
	struct usb_port *port_dev = hub->ports[port1 - 1];
	int status;

	status = set_port_feature(hub->hdev, (selector << 8) | port1,
			USB_PORT_FEAT_INDICATOR);
	dev_dbg(&port_dev->dev, "indicator %s status %d\n",
		to_led_name(selector), status);
}