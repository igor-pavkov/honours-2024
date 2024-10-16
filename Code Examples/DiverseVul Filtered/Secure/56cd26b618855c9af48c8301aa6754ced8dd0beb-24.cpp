static int __init serial_ir_init(void)
{
	int result;

	result = platform_driver_register(&serial_ir_driver);
	if (result)
		return result;

	serial_ir.pdev = platform_device_alloc("serial_ir", 0);
	if (!serial_ir.pdev) {
		result = -ENOMEM;
		goto exit_driver_unregister;
	}

	result = platform_device_add(serial_ir.pdev);
	if (result)
		goto exit_device_put;

	return 0;

exit_device_put:
	platform_device_put(serial_ir.pdev);
exit_driver_unregister:
	platform_driver_unregister(&serial_ir_driver);
	return result;
}