static void i2c_ddc_init(Object *obj)
{
    I2CDDCState *s = I2CDDC(obj);

    qemu_edid_generate(s->edid_blob, sizeof(s->edid_blob), &s->edid_info);
}