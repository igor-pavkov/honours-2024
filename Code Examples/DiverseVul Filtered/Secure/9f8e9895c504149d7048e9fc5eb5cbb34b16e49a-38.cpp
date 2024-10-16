static void usb_bus_class_init(ObjectClass *klass, void *data)
{
    BusClass *k = BUS_CLASS(klass);

    k->print_dev = usb_bus_dev_print;
    k->get_dev_path = usb_get_dev_path;
    k->get_fw_dev_path = usb_get_fw_dev_path;
}