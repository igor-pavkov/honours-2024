static void qdev_e1000_reset(DeviceState *dev)
{
    E1000State *d = E1000(dev);
    e1000_reset(d);
}