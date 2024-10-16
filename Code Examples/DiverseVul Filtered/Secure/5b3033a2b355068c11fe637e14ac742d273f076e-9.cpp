void send_error_to_client_(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status)
{
	if(!decoder->private_->is_seeking)
		decoder->private_->error_callback(decoder, status, decoder->private_->client_data);
	else if(status == FLAC__STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM)
		decoder->private_->unparseable_frame_count++;
}