static void __exit serial_ir_exit_module(void)
{
	del_timer_sync(&serial_ir.timeout_timer);
	serial_ir_exit();
}