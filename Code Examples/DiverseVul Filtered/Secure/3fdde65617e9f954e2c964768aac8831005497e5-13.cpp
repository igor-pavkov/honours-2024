static int exif_process_IFD_in_JPEG(image_info_type *ImageInfo, char *dir_start, char *offset_base, size_t IFDlength, size_t displacement, int section_index, int tag)
{
	int de;
	int NumDirEntries;
	int NextDirOffset = 0;

#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Process %s (x%04X(=%d))", exif_get_sectionname(section_index), IFDlength, IFDlength);
#endif

	ImageInfo->sections_found |= FOUND_IFD0;

	if ((dir_start + 2) > (offset_base+IFDlength)) {
		exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Illegal IFD size");
		return FALSE;
	}

	NumDirEntries = php_ifd_get16u(dir_start, ImageInfo->motorola_intel);

	if ((dir_start+2+NumDirEntries*12) > (offset_base+IFDlength)) {
		exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Illegal IFD size: x%04X + 2 + x%04X*12 = x%04X > x%04X", (int)((size_t)dir_start+2-(size_t)offset_base), NumDirEntries, (int)((size_t)dir_start+2+NumDirEntries*12-(size_t)offset_base), IFDlength);
		return FALSE;
	}

	for (de=0;de<NumDirEntries;de++) {
		if (!exif_process_IFD_TAG(ImageInfo, dir_start + 2 + 12 * de,
								  offset_base, IFDlength, displacement, section_index, 1, exif_get_tag_table(section_index))) {
			return FALSE;
		}
	}
	/*
	 * Ignore IFD2 if it purportedly exists
	 */
	if (section_index == SECTION_THUMBNAIL) {
		return TRUE;
	}
	/*
	 * Hack to make it process IDF1 I hope
	 * There are 2 IDFs, the second one holds the keys (0x0201 and 0x0202) to the thumbnail
	 */
	if ((dir_start+2+12*de + 4) > (offset_base+IFDlength)) {
		exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Illegal IFD size");
		return FALSE;
	}

	if (tag != TAG_EXIF_IFD_POINTER && tag != TAG_GPS_IFD_POINTER) {
		NextDirOffset = php_ifd_get32u(dir_start+2+12*de, ImageInfo->motorola_intel);
	}

	if (NextDirOffset) {
		/* the next line seems false but here IFDlength means length of all IFDs */
		if (offset_base + NextDirOffset < offset_base || offset_base + NextDirOffset > offset_base+IFDlength) {
			exif_error_docref("exif_read_data#error_ifd" EXIFERR_CC, ImageInfo, E_WARNING, "Illegal IFD offset");
			return FALSE;
		}
		/* That is the IFD for the first thumbnail */
#ifdef EXIF_DEBUG
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Expect next IFD to be thumbnail");
#endif
		if (exif_process_IFD_in_JPEG(ImageInfo, offset_base + NextDirOffset, offset_base, IFDlength, displacement, SECTION_THUMBNAIL, 0)) {
#ifdef EXIF_DEBUG
			exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Thumbnail size: 0x%04X", ImageInfo->Thumbnail.size);
#endif
			if (ImageInfo->Thumbnail.filetype != IMAGE_FILETYPE_UNKNOWN
			&&  ImageInfo->Thumbnail.size
			&&  ImageInfo->Thumbnail.offset
			&&  ImageInfo->read_thumbnail
			) {
				exif_thumbnail_extract(ImageInfo, offset_base, IFDlength);
			}
			return TRUE;
		} else {
			return FALSE;
		}
	}
	return TRUE;
}