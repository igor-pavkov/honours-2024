static void usb_register_types(void)
{
    type_register_static(&usb_bus_info);
    type_register_static(&usb_device_type_info);
}