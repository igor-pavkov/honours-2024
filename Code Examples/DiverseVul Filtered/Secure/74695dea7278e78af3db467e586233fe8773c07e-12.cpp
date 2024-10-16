static GF_Err compress_sample_data(GF_NHMLDmxCtx *ctx, u32 compress_type, char **dict, u32 offset)
{
	z_stream stream;
	int err;
	u32 size;

	if (!ctx) return GF_OK;

	size = ctx->samp_buffer_size*ZLIB_COMPRESS_SAFE;
	if (ctx->zlib_buffer_alloc < size) {
		ctx->zlib_buffer_alloc = size;
		ctx->zlib_buffer = gf_realloc(ctx->zlib_buffer, sizeof(char)*size);
	}

	stream.next_in = (Bytef*) ctx->samp_buffer + offset;
	stream.avail_in = (uInt)ctx->samp_buffer_size - offset;
	stream.next_out = ( Bytef*)ctx->zlib_buffer;
	stream.avail_out = (uInt)size;
	stream.zalloc = (alloc_func)NULL;
	stream.zfree = (free_func)NULL;
	stream.opaque = (voidpf)NULL;

	if (compress_type==1) {
		err = deflateInit(&stream, 9);
	} else {
		err = deflateInit2(&stream, 9, Z_DEFLATED, 16+MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
	}
	if (err != Z_OK) {
		return GF_IO_ERR;
	}
	if (dict && *dict) {
		err = deflateSetDictionary(&stream, (Bytef *)*dict, (u32) strlen(*dict));
		if (err != Z_OK) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[NHMLDmx] Error assigning dictionary\n"));
			deflateEnd(&stream);
			return GF_IO_ERR;
		}
	}
	err = deflate(&stream, Z_FINISH);
	if (err != Z_STREAM_END) {
		deflateEnd(&stream);
		return GF_IO_ERR;
	}
	if (ctx->samp_buffer_size - offset < stream.total_out) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[NHMLDmx] compressed data (%d) bigger than input data (%d)\n", (u32) stream.total_out, (u32) ctx->samp_buffer_size - offset));
	}
	if (dict) {
		if (*dict) gf_free(*dict);
		*dict = (char*)gf_malloc(sizeof(char) * ctx->samp_buffer_size);
		memcpy(*dict, ctx->samp_buffer, ctx->samp_buffer_size);
	}
	if (ctx->samp_buffer_alloc < stream.total_out) {
		ctx->samp_buffer_alloc = (u32) (stream.total_out*2);
		ctx->samp_buffer = (char*)gf_realloc(ctx->samp_buffer, ctx->samp_buffer_alloc * sizeof(char));
	}

	memcpy(ctx->samp_buffer + offset, ctx->zlib_buffer, sizeof(char)*stream.total_out);
	ctx->samp_buffer_size = (u32) (offset + stream.total_out);

	deflateEnd(&stream);
	return GF_OK;
}