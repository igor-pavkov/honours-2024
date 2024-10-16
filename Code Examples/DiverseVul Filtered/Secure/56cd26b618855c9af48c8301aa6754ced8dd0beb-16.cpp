static void serial_ir_exit(void)
{
	platform_device_unregister(serial_ir.pdev);
	platform_driver_unregister(&serial_ir_driver);
}