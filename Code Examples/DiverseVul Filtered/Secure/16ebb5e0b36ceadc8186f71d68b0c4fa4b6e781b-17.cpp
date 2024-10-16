static u32 qdisc_alloc_handle(struct net_device *dev)
{
	int i = 0x10000;
	static u32 autohandle = TC_H_MAKE(0x80000000U, 0);

	do {
		autohandle += TC_H_MAKE(0x10000U, 0);
		if (autohandle == TC_H_MAKE(TC_H_ROOT, 0))
			autohandle = TC_H_MAKE(0x80000000U, 0);
	} while	(qdisc_lookup(dev, autohandle) && --i > 0);

	return i>0 ? autohandle : 0;
}