GF_Box *trun_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TrackFragmentRunBox, GF_ISOM_BOX_TYPE_TRUN);
	//NO FLAGS SET BY DEFAULT
	return (GF_Box *)tmp;
}