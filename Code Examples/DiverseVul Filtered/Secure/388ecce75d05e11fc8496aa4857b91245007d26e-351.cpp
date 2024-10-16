GF_Box *mvhd_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_MovieHeaderBox, GF_ISOM_BOX_TYPE_MVHD);

	tmp->preferredRate = (1<<16);
	tmp->preferredVolume = (1<<8);

	tmp->matrixA = (1<<16);
	tmp->matrixD = (1<<16);
	tmp->matrixW = (1<<30);

	tmp->nextTrackID = 1;

	return (GF_Box *)tmp;
}