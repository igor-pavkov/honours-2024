void EVP_EncodeInit(EVP_ENCODE_CTX *ctx)
	{
	ctx->length=48;
	ctx->num=0;
	ctx->line_num=0;
	}