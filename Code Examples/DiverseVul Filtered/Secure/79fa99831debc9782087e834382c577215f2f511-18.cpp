void hda_codec_bus_init(DeviceState *dev, HDACodecBus *bus, size_t bus_size,
                        hda_codec_response_func response,
                        hda_codec_xfer_func xfer)
{
    qbus_init(bus, bus_size, TYPE_HDA_BUS, dev, NULL);
    bus->response = response;
    bus->xfer = xfer;
}