static struct istream *quoted_string_istream_create
(struct managesieve_parser *parser)
{
	struct quoted_string_istream *qsstream;

	qsstream = i_new(struct quoted_string_istream, 1);
	qsstream->istream.max_buffer_size =
		parser->input->real_stream->max_buffer_size;
	qsstream->istream.read = quoted_string_istream_read;

	qsstream->istream.istream.readable_fd = FALSE;
	qsstream->istream.istream.blocking = parser->input->blocking;
	qsstream->istream.istream.seekable = FALSE;
	return i_stream_create(&qsstream->istream, parser->input,
			       i_stream_get_fd(parser->input), 0);
}