static int TIFFWriteDirectoryTagFloat(TIFF* tif, uint32* ndir, TIFFDirEntry* dir, uint16 tag, float value)
{
	if (dir==NULL)
	{
		(*ndir)++;
		return(1);
	}
	return(TIFFWriteDirectoryTagCheckedFloat(tif,ndir,dir,tag,value));
}