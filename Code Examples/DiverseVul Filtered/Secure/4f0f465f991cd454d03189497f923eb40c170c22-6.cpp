MODULE_ENTRY (fill_info) (GdkPixbufFormat *info)
{
	static GdkPixbufModulePattern signature[] = {
		{ "#define ", NULL, 100 },
		{ "/*", NULL, 50 },
		{ NULL, NULL, 0 }
	};
	static gchar * mime_types[] = {
		"image/x-xbitmap",
		NULL
	};
	static gchar * extensions[] = {
		"xbm",
		NULL
	};

	info->name = "xbm";
	info->signature = signature;
	info->description = N_("The XBM image format");
	info->mime_types = mime_types;
	info->extensions = extensions;
	info->flags = GDK_PIXBUF_FORMAT_THREADSAFE;
	info->license = "LGPL";
}