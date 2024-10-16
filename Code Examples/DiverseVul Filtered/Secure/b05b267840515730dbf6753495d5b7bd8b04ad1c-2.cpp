static void i2c_ddc_class_init(ObjectClass *oc, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);
    I2CSlaveClass *isc = I2C_SLAVE_CLASS(oc);

    dc->reset = i2c_ddc_reset;
    dc->vmsd = &vmstate_i2c_ddc;
    dc->props = i2c_ddc_properties;
    isc->event = i2c_ddc_event;
    isc->recv = i2c_ddc_rx;
    isc->send = i2c_ddc_tx;
}