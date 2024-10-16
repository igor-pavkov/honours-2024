gerbv_image_t *merge_images (int type)
{
	gint i, filecount, img;
	gerbv_image_t *out;
	gerbv_layertype_t layertype;
	struct l_image_info {
		gerbv_image_t *image;
		gerbv_user_transformation_t *transform;
	} *images;
	
	images=(struct l_image_info *)g_new0(struct l_image_info,1);
	out = NULL;
	switch(type) {
	case CALLBACKS_SAVE_FILE_DRILLM:
		layertype = GERBV_LAYERTYPE_DRILL;
		break;
	case CALLBACKS_SAVE_FILE_RS274XM:
		layertype = GERBV_LAYERTYPE_RS274X;
		break;
	default:
		GERB_COMPILE_ERROR(_("Unknown Layer type for merge"));
		goto err;
	}
	dprintf("Looking for matching files\n");
	for (i = img = filecount = 0; i < mainProject->max_files; ++i) {
		if (mainProject->file[i] &&  mainProject->file[i]->isVisible &&
		(mainProject->file[i]->image->layertype == layertype)) {
			++filecount;
			dprintf("Adding '%s'\n", mainProject->file[i]->name);
			images[img].image=mainProject->file[i]->image;
			images[img++].transform=&mainProject->file[i]->transform;
			images = (struct l_image_info *)g_renew(struct l_image_info, images, img+1);
		}
	}
	if (filecount < 2) {
		GERB_COMPILE_ERROR(_("Not Enough Files of same type to merge"));
		goto err;
	}
	dprintf("Now merging files\n");
	for (i = 0; i < img; ++i) {
		gerbv_user_transformation_t *thisTransform;
		gerbv_user_transformation_t identityTransform = {0,0,1,1,0,FALSE,FALSE,FALSE};
		thisTransform=images[i].transform;
		if (NULL == thisTransform)
			thisTransform = &identityTransform;
		if (0 == i)
			out = gerbv_image_duplicate_image(images[i].image, thisTransform);
		else
			gerbv_image_copy_image(images[i].image, thisTransform, out);
	}
err:
	g_free(images);
	return out;
}