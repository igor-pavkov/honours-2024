Bool gf_laser_decode_has_conditionnals(GF_LASeRCodec *codec)
{
	return codec && codec->has_conditionnals ? GF_TRUE : GF_FALSE;
}