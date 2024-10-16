static int exif_read_from_stream(image_info_type *ImageInfo, php_stream *stream, int read_thumbnail, int read_all)
{
	int ret;
	off_t old_pos = php_stream_tell(stream);

	if (old_pos) {
		php_stream_seek(stream, 0, SEEK_SET);
	}

	ret = exif_read_from_impl(ImageInfo, stream, read_thumbnail, read_all);

	if (old_pos) {
		php_stream_seek(stream, old_pos, SEEK_SET);
	}

	return ret;
}