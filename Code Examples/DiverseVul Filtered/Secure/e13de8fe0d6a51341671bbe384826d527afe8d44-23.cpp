static void __init of_unittest_overlay_i2c_12(void)
{
	/* device should enable */
	if (of_unittest_apply_overlay_check(12, 12, 0, 1, I2C_OVERLAY))
		return;

	unittest(1, "overlay test %d passed\n", 12);
}