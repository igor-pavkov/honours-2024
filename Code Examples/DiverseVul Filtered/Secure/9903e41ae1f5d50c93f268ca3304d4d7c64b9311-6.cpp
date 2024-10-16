static int __init hi3660_stub_clk_init(void)
{
	return platform_driver_register(&hi3660_stub_clk_driver);
}