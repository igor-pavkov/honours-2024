static int edge_create_sysfs_attrs(struct usb_serial_port *port)
{
	return device_create_file(&port->dev, &dev_attr_uart_mode);
}