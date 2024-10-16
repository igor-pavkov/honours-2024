static int exif_read_from_file(image_info_type *ImageInfo, char *FileName, int read_thumbnail, int read_all)
{
	int ret;
	php_stream *stream;

	stream = php_stream_open_wrapper(FileName, "rb", STREAM_MUST_SEEK | IGNORE_PATH, NULL);

	if (!stream) {
		memset(&ImageInfo, 0, sizeof(ImageInfo));

		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Unable to open file");

		return FALSE;
	}

	ret = exif_read_from_stream(ImageInfo, stream, read_thumbnail, read_all);

	php_stream_close(stream);

	return ret;
}