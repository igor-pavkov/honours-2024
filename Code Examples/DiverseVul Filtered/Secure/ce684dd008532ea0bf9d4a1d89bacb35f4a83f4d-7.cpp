vips_foreign_load_gif_dispose( GObject *gobject )
{
	VipsForeignLoadGif *gif = (VipsForeignLoadGif *) gobject;

	vips_foreign_load_gif_close( gif ); 

	VIPS_UNREF( gif->frame ); 
	VIPS_UNREF( gif->previous ); 
	VIPS_FREE( gif->comment ); 
	VIPS_FREE( gif->line ) 

	G_OBJECT_CLASS( vips_foreign_load_gif_parent_class )->
		dispose( gobject );
}