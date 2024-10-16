static int decode_pixdata(deark *c, lctx *d, struct fmtutil_macbitmap_info *bi, i64 pos)
{
	int retval = 0;

	de_dbg_indent(c, 1);

	if(bi->npwidth==0 || bi->height==0) {
		de_warn(c, "Ignoring zero-size bitmap (%d"DE_CHAR_TIMES"%d)",
			(int)bi->npwidth, (int)bi->height);
		goto done;
	}
	if(!de_good_image_dimensions(c, bi->npwidth, bi->height)) goto done;

	if(bi->pixelsize!=1 && bi->pixelsize!=2 && bi->pixelsize!=4 && bi->pixelsize!=8 &&
		bi->pixelsize!=16 && bi->pixelsize!=24 && bi->pixelsize!=32)
	{
		de_err(c, "%d bits/pixel images are not supported", (int)bi->pixelsize);
		goto done;
	}
	if((bi->uses_pal && bi->pixeltype!=0) || (!bi->uses_pal && bi->pixeltype!=16)) {
		de_err(c, "Pixel type %d is not supported", (int)bi->pixeltype);
		goto done;
	}
	if(bi->cmpcount!=1 && bi->cmpcount!=3 && bi->cmpcount!=4) {
		de_err(c, "Component count %d is not supported", (int)bi->cmpcount);
		goto done;
	}
	if(bi->cmpsize!=1 && bi->cmpsize!=2 && bi->cmpsize!=4 && bi->cmpsize!=5 &&
		bi->cmpsize!=8)
	{
		de_err(c, "%d-bit components are not supported", (int)bi->cmpsize);
		goto done;
	}
	if(bi->packing_type!=0 && bi->packing_type!=1 && bi->packing_type!=3 && bi->packing_type!=4) {
		de_err(c, "Packing type %d is not supported", (int)bi->packing_type);
		goto done;
	}
	if((bi->uses_pal &&
		(bi->packing_type==0 || bi->packing_type==1) &&
		(bi->pixelsize==1 || bi->pixelsize==2 || bi->pixelsize==4 || bi->pixelsize==8) &&
		bi->cmpcount==1 && bi->cmpsize==bi->pixelsize) ||
		(!bi->uses_pal && bi->packing_type==3 && bi->pixelsize==16 && bi->cmpcount==3 && bi->cmpsize==5) ||
		(!bi->uses_pal && bi->packing_type==4 && bi->pixelsize==32 && bi->cmpcount==3 && bi->cmpsize==8) ||
		(!bi->uses_pal && bi->packing_type==4 && bi->pixelsize==32 && bi->cmpcount==4 && bi->cmpsize==8))
	{
		;
	}
	else {
		de_err(c, "This type of image is not supported");
		goto done;
	}

	if(bi->cmpcount==4) {
		de_warn(c, "This image might have transparency, which is not supported.");
	}

	decode_bitmap(c, d, bi, pos);

done:
	de_dbg_indent(c, -1);
	return retval;
}