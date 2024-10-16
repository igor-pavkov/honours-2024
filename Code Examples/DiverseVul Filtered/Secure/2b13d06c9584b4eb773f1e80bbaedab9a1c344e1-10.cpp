static inline port_t* dev_to_port(struct net_device *dev)
{
        return (port_t *)dev_to_hdlc(dev)->priv;
}