void t2p_compose_pdf_page(T2P* t2p){

	uint32 i=0;
	uint32 i2=0;
	T2P_TILE* tiles=NULL;
	T2P_BOX* boxp=NULL;
	uint32 tilecountx=0;
	uint32 tilecounty=0;
	uint32 tilewidth=0;
	uint32 tilelength=0;
	int istiled=0;
	float f=0;
	float width_ratio=0;
	float length_ratio=0;
	
	t2p->pdf_xres = t2p->tiff_xres;
	t2p->pdf_yres = t2p->tiff_yres;
	if(t2p->pdf_overrideres) {
		t2p->pdf_xres = t2p->pdf_defaultxres;
		t2p->pdf_yres = t2p->pdf_defaultyres;
	}
	if(t2p->pdf_xres == 0.0)
		t2p->pdf_xres = t2p->pdf_defaultxres;
	if(t2p->pdf_yres == 0.0)
		t2p->pdf_yres = t2p->pdf_defaultyres;
	if (t2p->pdf_image_fillpage) {
		width_ratio = t2p->pdf_defaultpagewidth/t2p->tiff_width;
		length_ratio = t2p->pdf_defaultpagelength/t2p->tiff_length;
		if (width_ratio < length_ratio ) {
			t2p->pdf_imagewidth = t2p->pdf_defaultpagewidth;
			t2p->pdf_imagelength = t2p->tiff_length * width_ratio;
		} else {
			t2p->pdf_imagewidth = t2p->tiff_width * length_ratio;
			t2p->pdf_imagelength = t2p->pdf_defaultpagelength;
		}
	} else if (t2p->tiff_resunit != RESUNIT_CENTIMETER	/* RESUNIT_NONE and */
		&& t2p->tiff_resunit != RESUNIT_INCH) {	/* other cases */
		t2p->pdf_imagewidth = ((float)(t2p->tiff_width))/t2p->pdf_xres;
		t2p->pdf_imagelength = ((float)(t2p->tiff_length))/t2p->pdf_yres;
	} else {
		t2p->pdf_imagewidth = 
			((float)(t2p->tiff_width))*PS_UNIT_SIZE/t2p->pdf_xres;
		t2p->pdf_imagelength = 
			((float)(t2p->tiff_length))*PS_UNIT_SIZE/t2p->pdf_yres;
	}
	if(t2p->pdf_overridepagesize != 0) {
		t2p->pdf_pagewidth = t2p->pdf_defaultpagewidth;
		t2p->pdf_pagelength = t2p->pdf_defaultpagelength;
	} else {
		t2p->pdf_pagewidth = t2p->pdf_imagewidth;
		t2p->pdf_pagelength = t2p->pdf_imagelength;
	}
	t2p->pdf_mediabox.x1=0.0;
	t2p->pdf_mediabox.y1=0.0;
	t2p->pdf_mediabox.x2=t2p->pdf_pagewidth;
	t2p->pdf_mediabox.y2=t2p->pdf_pagelength;
	t2p->pdf_imagebox.x1=0.0;
	t2p->pdf_imagebox.y1=0.0;
	t2p->pdf_imagebox.x2=t2p->pdf_imagewidth;
	t2p->pdf_imagebox.y2=t2p->pdf_imagelength;
	if(t2p->pdf_overridepagesize!=0){
		t2p->pdf_imagebox.x1+=((t2p->pdf_pagewidth-t2p->pdf_imagewidth)/2.0F);
		t2p->pdf_imagebox.y1+=((t2p->pdf_pagelength-t2p->pdf_imagelength)/2.0F);
		t2p->pdf_imagebox.x2+=((t2p->pdf_pagewidth-t2p->pdf_imagewidth)/2.0F);
		t2p->pdf_imagebox.y2+=((t2p->pdf_pagelength-t2p->pdf_imagelength)/2.0F);
	}
	if(t2p->tiff_orientation > 4){
		f=t2p->pdf_mediabox.x2;
		t2p->pdf_mediabox.x2=t2p->pdf_mediabox.y2;
		t2p->pdf_mediabox.y2=f;
	}
	istiled=((t2p->tiff_tiles[t2p->pdf_page]).tiles_tilecount==0) ? 0 : 1;
	if(istiled==0){
		t2p_compose_pdf_page_orient(&(t2p->pdf_imagebox), t2p->tiff_orientation);
		return;
	} else {
		tilewidth=(t2p->tiff_tiles[t2p->pdf_page]).tiles_tilewidth;
		tilelength=(t2p->tiff_tiles[t2p->pdf_page]).tiles_tilelength;
		if( tilewidth > INT_MAX ||
		    tilelength > INT_MAX ||
		    t2p->tiff_width > INT_MAX - tilewidth ||
		    t2p->tiff_length > INT_MAX - tilelength )
		{
		    TIFFError(TIFF2PDF_MODULE, "Integer overflow");
		    t2p->t2p_error = T2P_ERR_ERROR;
		    return;
		}
		tilecountx=(t2p->tiff_width + 
			tilewidth -1)/ 
			tilewidth;
		(t2p->tiff_tiles[t2p->pdf_page]).tiles_tilecountx=tilecountx;
		tilecounty=(t2p->tiff_length + 
			tilelength -1)/ 
			tilelength;
		(t2p->tiff_tiles[t2p->pdf_page]).tiles_tilecounty=tilecounty;
		(t2p->tiff_tiles[t2p->pdf_page]).tiles_edgetilewidth=
			t2p->tiff_width % tilewidth;
		(t2p->tiff_tiles[t2p->pdf_page]).tiles_edgetilelength=
			t2p->tiff_length % tilelength;
		tiles=(t2p->tiff_tiles[t2p->pdf_page]).tiles_tiles;
		for(i2=0;i2<tilecounty-1;i2++){
			for(i=0;i<tilecountx-1;i++){
				boxp=&(tiles[i2*tilecountx+i].tile_box);
				boxp->x1 = 
					t2p->pdf_imagebox.x1 
					+ ((float)(t2p->pdf_imagewidth * i * tilewidth)
					/ (float)t2p->tiff_width);
				boxp->x2 = 
					t2p->pdf_imagebox.x1 
					+ ((float)(t2p->pdf_imagewidth * (i+1) * tilewidth)
					/ (float)t2p->tiff_width);
				boxp->y1 = 
					t2p->pdf_imagebox.y2 
					- ((float)(t2p->pdf_imagelength * (i2+1) * tilelength)
					/ (float)t2p->tiff_length);
				boxp->y2 = 
					t2p->pdf_imagebox.y2 
					- ((float)(t2p->pdf_imagelength * i2 * tilelength)
					/ (float)t2p->tiff_length);
			}
			boxp=&(tiles[i2*tilecountx+i].tile_box);
			boxp->x1 = 
				t2p->pdf_imagebox.x1 
				+ ((float)(t2p->pdf_imagewidth * i * tilewidth)
				/ (float)t2p->tiff_width);
			boxp->x2 = t2p->pdf_imagebox.x2;
			boxp->y1 = 
				t2p->pdf_imagebox.y2 
				- ((float)(t2p->pdf_imagelength * (i2+1) * tilelength)
				/ (float)t2p->tiff_length);
			boxp->y2 = 
				t2p->pdf_imagebox.y2 
				- ((float)(t2p->pdf_imagelength * i2 * tilelength)
				/ (float)t2p->tiff_length);
		}
		for(i=0;i<tilecountx-1;i++){
			boxp=&(tiles[i2*tilecountx+i].tile_box);
			boxp->x1 = 
				t2p->pdf_imagebox.x1 
				+ ((float)(t2p->pdf_imagewidth * i * tilewidth)
				/ (float)t2p->tiff_width);
			boxp->x2 = 
				t2p->pdf_imagebox.x1 
				+ ((float)(t2p->pdf_imagewidth * (i+1) * tilewidth)
				/ (float)t2p->tiff_width);
			boxp->y1 = t2p->pdf_imagebox.y1;
			boxp->y2 = 
				t2p->pdf_imagebox.y2 
				- ((float)(t2p->pdf_imagelength * i2 * tilelength)
				/ (float)t2p->tiff_length);
		}
		boxp=&(tiles[i2*tilecountx+i].tile_box);
		boxp->x1 = 
			t2p->pdf_imagebox.x1 
			+ ((float)(t2p->pdf_imagewidth * i * tilewidth)
			/ (float)t2p->tiff_width);
		boxp->x2 = t2p->pdf_imagebox.x2;
		boxp->y1 = t2p->pdf_imagebox.y1;
		boxp->y2 = 
			t2p->pdf_imagebox.y2 
			- ((float)(t2p->pdf_imagelength * i2 * tilelength)
			/ (float)t2p->tiff_length);
	}
	if(t2p->tiff_orientation==0 || t2p->tiff_orientation==1){
		for(i=0;i<(t2p->tiff_tiles[t2p->pdf_page]).tiles_tilecount;i++){
			t2p_compose_pdf_page_orient( &(tiles[i].tile_box) , 0);
		}
		return;
	}
	for(i=0;i<(t2p->tiff_tiles[t2p->pdf_page]).tiles_tilecount;i++){
		boxp=&(tiles[i].tile_box);
		boxp->x1 -= t2p->pdf_imagebox.x1;
		boxp->x2 -= t2p->pdf_imagebox.x1;
		boxp->y1 -= t2p->pdf_imagebox.y1;
		boxp->y2 -= t2p->pdf_imagebox.y1;
		if(t2p->tiff_orientation==2 || t2p->tiff_orientation==3){
			boxp->x1 = t2p->pdf_imagebox.x2 - t2p->pdf_imagebox.x1 - boxp->x1;
			boxp->x2 = t2p->pdf_imagebox.x2 - t2p->pdf_imagebox.x1 - boxp->x2;
		}
		if(t2p->tiff_orientation==3 || t2p->tiff_orientation==4){
			boxp->y1 = t2p->pdf_imagebox.y2 - t2p->pdf_imagebox.y1 - boxp->y1;
			boxp->y2 = t2p->pdf_imagebox.y2 - t2p->pdf_imagebox.y1 - boxp->y2;
		}
		if(t2p->tiff_orientation==8 || t2p->tiff_orientation==5){
			boxp->y1 = t2p->pdf_imagebox.y2 - t2p->pdf_imagebox.y1 - boxp->y1;
			boxp->y2 = t2p->pdf_imagebox.y2 - t2p->pdf_imagebox.y1 - boxp->y2;
		}
		if(t2p->tiff_orientation==5 || t2p->tiff_orientation==6){
			boxp->x1 = t2p->pdf_imagebox.x2 - t2p->pdf_imagebox.x1 - boxp->x1;
			boxp->x2 = t2p->pdf_imagebox.x2 - t2p->pdf_imagebox.x1 - boxp->x2;
		}
		if(t2p->tiff_orientation > 4){
			f=boxp->x1;
			boxp->x1 = boxp->y1;
			boxp->y1 = f;
			f=boxp->x2;
			boxp->x2 = boxp->y2;
			boxp->y2 = f; 
			t2p_compose_pdf_page_orient_flip(boxp, t2p->tiff_orientation);
		} else {
			t2p_compose_pdf_page_orient(boxp, t2p->tiff_orientation);
		}
		
	}

	return;
}