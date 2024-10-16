FLAC_API FLAC__bool FLAC__stream_decoder_process_until_end_of_stream(FLAC__StreamDecoder *decoder)
{
	FLAC__bool dummy;
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->protected_);

	while(1) {
		switch(decoder->protected_->state) {
			case FLAC__STREAM_DECODER_SEARCH_FOR_METADATA:
				if(!find_metadata_(decoder))
					return false; /* above function sets the status for us */
				break;
			case FLAC__STREAM_DECODER_READ_METADATA:
				if(!read_metadata_(decoder))
					return false; /* above function sets the status for us */
				break;
			case FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC:
				if(!frame_sync_(decoder))
					return true; /* above function sets the status for us */
				break;
			case FLAC__STREAM_DECODER_READ_FRAME:
				if(!read_frame_(decoder, &dummy, /*do_full_decode=*/true))
					return false; /* above function sets the status for us */
				break;
			case FLAC__STREAM_DECODER_END_OF_STREAM:
			case FLAC__STREAM_DECODER_ABORTED:
				return true;
			default:
				FLAC__ASSERT(0);
				return false;
		}
	}
}