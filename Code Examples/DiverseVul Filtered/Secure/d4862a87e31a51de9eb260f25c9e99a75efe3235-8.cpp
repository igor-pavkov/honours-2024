static void pit_class_initfn(ObjectClass *klass, void *data)
{
    PITClass *pc = PIT_CLASS(klass);
    PITCommonClass *k = PIT_COMMON_CLASS(klass);
    DeviceClass *dc = DEVICE_CLASS(klass);

    pc->parent_realize = dc->realize;
    dc->realize = pit_realizefn;
    k->set_channel_gate = pit_set_channel_gate;
    k->get_channel_info = pit_get_channel_info_common;
    k->post_load = pit_post_load;
    dc->reset = pit_reset;
    dc->props = pit_properties;
}