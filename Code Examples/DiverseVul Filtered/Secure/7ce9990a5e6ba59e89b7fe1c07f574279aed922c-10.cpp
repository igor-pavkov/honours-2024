static ssize_t quoted_string_istream_read(struct istream_private *stream)
{
	struct quoted_string_istream *qsstream =
		(struct quoted_string_istream *)stream;
	const unsigned char *data;
	unsigned int extra;
	size_t i, dest, size;
	ssize_t ret;

	if (qsstream->str_end) {
		stream->istream.eof = TRUE;
		return -1;
	}

	ret = quoted_string_istream_read_parent(qsstream, 1);
	if (ret <= 0)
		return ret;

	/* @UNSAFE */
	dest = stream->pos;
	extra = 0;

	data = i_stream_get_data(stream->parent, &size);
	for (i = 0; i < size && dest < stream->buffer_size; ) {
		if (data[i] == '"') {
			i++;
			qsstream->str_end = TRUE;
			if (dest == stream->pos) {
				i_stream_skip(stream->parent, i);
				stream->istream.eof = TRUE;
				return -1;
			}
			break;
		} else if (data[i] == '\\') {
			if (i+1 == size) {
				/* not enough input for \x */
				extra = 1;
				break;
			}
			i++;

			if (!IS_QUOTED_SPECIAL(data[i])) {
				/* invalid string */
				io_stream_set_error(&stream->iostream,
					"Escaped quoted-string character is not a QUOTED-SPECIAL");
				stream->istream.stream_errno = EINVAL;
				return -1;
			}
			stream->w_buffer[dest++] = data[i];
			i++;
		} else {
			if (data[i] == '\r' || data[i] == '\n') {
				io_stream_set_error(&stream->iostream,
					"Quoted string contains an invalid character");
				stream->istream.stream_errno = EINVAL;
				return -1;
			}

			stream->w_buffer[dest++] = data[i];
			i++;
		}
		i_assert(dest <= stream->buffer_size);
	}
	i_stream_skip(stream->parent, i);

	ret = dest - stream->pos;
	if (ret == 0) {
		/* not enough input */
		i_assert(i == 0);
		i_assert(extra > 0);
		ret = quoted_string_istream_read_parent(qsstream, extra+1);
		if (ret <= 0)
			return ret;
		return quoted_string_istream_read(stream);
	}
	i_assert(ret > 0);
	stream->pos = dest;
	return ret;
}