static void lan9118_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = lan9118_reset;
    device_class_set_props(dc, lan9118_properties);
    dc->vmsd = &vmstate_lan9118;
    dc->realize = lan9118_realize;
}