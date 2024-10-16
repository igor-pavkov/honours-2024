void EVP_EncodeFinal(EVP_ENCODE_CTX *ctx, unsigned char *out, int *outl)
	{
	unsigned int ret=0;

	if (ctx->num != 0)
		{
		ret=EVP_EncodeBlock(out,ctx->enc_data,ctx->num);
		out[ret++]='\n';
		out[ret]='\0';
		ctx->num=0;
		}
	*outl=ret;
	}