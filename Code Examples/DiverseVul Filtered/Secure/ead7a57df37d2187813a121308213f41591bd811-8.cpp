static void ssd0323_class_init(ObjectClass *klass, void *data)
{
    SSISlaveClass *k = SSI_SLAVE_CLASS(klass);

    k->init = ssd0323_init;
    k->transfer = ssd0323_transfer;
    k->cs_polarity = SSI_CS_HIGH;
}