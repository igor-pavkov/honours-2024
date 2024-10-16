static inline struct ppp* get_ppp(struct net_device *dev)
{
	return (struct ppp *)dev_to_hdlc(dev)->state;
}