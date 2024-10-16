static void exif_thumbnail_build(image_info_type *ImageInfo) {
	size_t            new_size, new_move, new_value;
	char              *new_data;
	void              *value_ptr;
	int               i, byte_count;
	image_info_list   *info_list;
	image_info_data   *info_data;
#ifdef EXIF_DEBUG
	char              tagname[64];
#endif

	if (!ImageInfo->read_thumbnail || !ImageInfo->Thumbnail.offset || !ImageInfo->Thumbnail.size) {
		return; /* ignore this call */
	}
#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Thumbnail: filetype = %d", ImageInfo->Thumbnail.filetype);
#endif
	switch(ImageInfo->Thumbnail.filetype) {
		default:
		case IMAGE_FILETYPE_JPEG:
			/* done */
			break;
		case IMAGE_FILETYPE_TIFF_II:
		case IMAGE_FILETYPE_TIFF_MM:
			info_list = &ImageInfo->info_list[SECTION_THUMBNAIL];
			new_size  = 8 + 2 + info_list->count * 12 + 4;
#ifdef EXIF_DEBUG
			exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Thumbnail: size of signature + directory(%d): 0x%02X", info_list->count, new_size);
#endif
			new_value= new_size; /* offset for ifd values outside ifd directory */
			for (i=0; i<info_list->count; i++) {
				info_data  = &info_list->list[i];
				byte_count = php_tiff_bytes_per_format[info_data->format] * info_data->length;
				if (byte_count > 4) {
					new_size += byte_count;
				}
			}
			new_move = new_size;
			new_data = safe_erealloc(ImageInfo->Thumbnail.data, 1, ImageInfo->Thumbnail.size, new_size);
			ImageInfo->Thumbnail.data = new_data;
			memmove(ImageInfo->Thumbnail.data + new_move, ImageInfo->Thumbnail.data, ImageInfo->Thumbnail.size);
			ImageInfo->Thumbnail.size += new_size;
			/* fill in data */
			if (ImageInfo->motorola_intel) {
				memmove(new_data, "MM\x00\x2a\x00\x00\x00\x08", 8);
			} else {
				memmove(new_data, "II\x2a\x00\x08\x00\x00\x00", 8);
			}
			new_data += 8;
			php_ifd_set16u(new_data, info_list->count, ImageInfo->motorola_intel);
			new_data += 2;
			for (i=0; i<info_list->count; i++) {
				info_data  = &info_list->list[i];
				byte_count = php_tiff_bytes_per_format[info_data->format] * info_data->length;
#ifdef EXIF_DEBUG
				exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Thumbnail: process tag(x%04X=%s): %s%s (%d bytes)", info_data->tag, exif_get_tagname(info_data->tag, tagname, -12, tag_table_IFD), (info_data->length>1)&&info_data->format!=TAG_FMT_UNDEFINED&&info_data->format!=TAG_FMT_STRING?"ARRAY OF ":"", exif_get_tagformat(info_data->format), byte_count);
#endif
				if (info_data->tag==TAG_STRIP_OFFSETS || info_data->tag==TAG_JPEG_INTERCHANGE_FORMAT) {
					php_ifd_set16u(new_data + 0, info_data->tag,    ImageInfo->motorola_intel);
					php_ifd_set16u(new_data + 2, TAG_FMT_ULONG,     ImageInfo->motorola_intel);
					php_ifd_set32u(new_data + 4, 1,                 ImageInfo->motorola_intel);
					php_ifd_set32u(new_data + 8, new_move,          ImageInfo->motorola_intel);
				} else {
					php_ifd_set16u(new_data + 0, info_data->tag,    ImageInfo->motorola_intel);
					php_ifd_set16u(new_data + 2, info_data->format, ImageInfo->motorola_intel);
					php_ifd_set32u(new_data + 4, info_data->length, ImageInfo->motorola_intel);
					value_ptr  = exif_ifd_make_value(info_data, ImageInfo->motorola_intel);
					if (byte_count <= 4) {
						memmove(new_data+8, value_ptr, 4);
					} else {
						php_ifd_set32u(new_data+8, new_value, ImageInfo->motorola_intel);
#ifdef EXIF_DEBUG
						exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Thumbnail: writing with value offset: 0x%04X + 0x%02X", new_value, byte_count);
#endif
						memmove(ImageInfo->Thumbnail.data+new_value, value_ptr, byte_count);
						new_value += byte_count;
					}
					efree(value_ptr);
				}
				new_data += 12;
			}
			memset(new_data, 0, 4); /* next ifd pointer */
#ifdef EXIF_DEBUG
			exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Thumbnail: created");
#endif
			break;
	}
}