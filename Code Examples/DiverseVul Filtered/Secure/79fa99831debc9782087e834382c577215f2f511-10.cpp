void hda_codec_response(HDACodecDevice *dev, bool solicited, uint32_t response)
{
    HDACodecBus *bus = HDA_BUS(dev->qdev.parent_bus);
    bus->response(dev, solicited, response);
}