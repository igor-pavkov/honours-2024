bool hda_codec_xfer(HDACodecDevice *dev, uint32_t stnr, bool output,
                    uint8_t *buf, uint32_t len)
{
    HDACodecBus *bus = HDA_BUS(dev->qdev.parent_bus);
    return bus->xfer(dev, stnr, output, buf, len);
}