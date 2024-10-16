static void xgmac_enet_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = xgmac_enet_realize;
    dc->vmsd = &vmstate_xgmac;
    device_class_set_props(dc, xgmac_properties);
}