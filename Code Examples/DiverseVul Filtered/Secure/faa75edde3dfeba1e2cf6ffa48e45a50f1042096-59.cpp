void gf_laser_decoder_set_clock(GF_LASeRCodec *codec, Double (*GetSceneTime)(void *st_cbk), void *st_cbk )
{
	codec->GetSceneTime = GetSceneTime;
	codec->cbk = st_cbk;
}