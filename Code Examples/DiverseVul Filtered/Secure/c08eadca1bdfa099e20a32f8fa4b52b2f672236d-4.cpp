static void flush_request_modules(struct em28xx *dev)
{
	flush_work(&dev->request_module_wk);
}