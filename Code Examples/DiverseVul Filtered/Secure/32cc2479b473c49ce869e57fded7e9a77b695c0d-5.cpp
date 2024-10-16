static void tls1_md5_final_raw(void* ctx, unsigned char *md_out)
	{
	MD5_CTX *md5 = ctx;
	u32toLE(md5->A, md_out);
	u32toLE(md5->B, md_out);
	u32toLE(md5->C, md_out);
	u32toLE(md5->D, md_out);
	}