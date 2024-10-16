MODULE_ENTRY (fill_vtable) (GdkPixbufModule *module)
{
	module->load = gdk_pixbuf__xbm_image_load;
	module->begin_load = gdk_pixbuf__xbm_image_begin_load;
	module->stop_load = gdk_pixbuf__xbm_image_stop_load;
	module->load_increment = gdk_pixbuf__xbm_image_load_increment;
}