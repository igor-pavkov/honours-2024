Fixed BD_ReadSFFloat(GF_BifsDecoder * codec, GF_BitStream *bs)
{
	if (codec->ActiveQP && codec->ActiveQP->useEfficientCoding)
		return gf_bifs_dec_mantissa_float(codec, bs);

	return FLT2FIX(gf_bs_read_float(bs));
}