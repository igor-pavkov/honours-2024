static void ssi_sd_class_init(ObjectClass *klass, void *data)
{
    SSISlaveClass *k = SSI_SLAVE_CLASS(klass);

    k->init = ssi_sd_init;
    k->transfer = ssi_sd_transfer;
    k->cs_polarity = SSI_CS_LOW;
}