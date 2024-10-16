static int TIFFWriteDirectoryTagDoubleArray(TIFF* tif, uint32* ndir, TIFFDirEntry* dir, uint16 tag, uint32 count, double* value)
{
	if (dir==NULL)
	{
		(*ndir)++;
		return(1);
	}
	return(TIFFWriteDirectoryTagCheckedDoubleArray(tif,ndir,dir,tag,count,value));
}