static void exif_process_APP1(image_info_type *ImageInfo, char *CharBuf, size_t length, size_t displacement)
{
	/* Check the APP1 for Exif Identifier Code */
	static const uchar ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};
	if (length <= 8 || memcmp(CharBuf+2, ExifHeader, 6)) {
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Incorrect APP1 Exif Identifier Code");
		return;
	}
	exif_process_TIFF_in_JPEG(ImageInfo, CharBuf + 8, length - 8, displacement+8);
#ifdef EXIF_DEBUG
	exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Process APP1/EXIF done");
#endif
}