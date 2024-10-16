FLAC__bool file_eof_callback_(const FLAC__StreamDecoder *decoder, void *client_data)
{
	(void)client_data;

	return feof(decoder->private_->file)? true : false;
}