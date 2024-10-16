int EVP_DecodeFinal(EVP_ENCODE_CTX *ctx, unsigned char *out, int *outl)
	{
	int i;

	*outl=0;
	if (ctx->num != 0)
		{
		i=EVP_DecodeBlock(out,ctx->enc_data,ctx->num);
		if (i < 0) return(-1);
		ctx->num=0;
		*outl=i;
		return(1);
		}
	else
		return(1);
	}