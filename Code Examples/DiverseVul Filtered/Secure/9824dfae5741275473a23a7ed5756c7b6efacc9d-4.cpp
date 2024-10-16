static int __init ipddp_init_module(void)
{
	dev_ipddp = ipddp_init();
	return PTR_ERR_OR_ZERO(dev_ipddp);
}