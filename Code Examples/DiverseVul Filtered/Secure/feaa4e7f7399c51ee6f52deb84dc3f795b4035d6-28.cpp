static void swizzle_io_read(RKernelCacheObj *obj, RIO *io) {
	r_return_if_fail (io && io->desc && io->desc->plugin);
	RIOPlugin *plugin = io->desc->plugin;
	obj->original_io_read = plugin->read;
	plugin->read = &kernelcache_io_read;
}