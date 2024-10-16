static enum TIFFReadDirEntryErr TIFFReadDirEntryFloat(TIFF* tif, TIFFDirEntry* direntry, float* value)
{
	enum TIFFReadDirEntryErr err;
	if (direntry->tdir_count!=1)
		return(TIFFReadDirEntryErrCount);
	switch (direntry->tdir_type)
	{
		case TIFF_BYTE:
			{
				uint8 m;
				TIFFReadDirEntryCheckedByte(tif,direntry,&m);
				*value=(float)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_SBYTE:
			{
				int8 m;
				TIFFReadDirEntryCheckedSbyte(tif,direntry,&m);
				*value=(float)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_SHORT:
			{
				uint16 m;
				TIFFReadDirEntryCheckedShort(tif,direntry,&m);
				*value=(float)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_SSHORT:
			{
				int16 m;
				TIFFReadDirEntryCheckedSshort(tif,direntry,&m);
				*value=(float)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_LONG:
			{
				uint32 m;
				TIFFReadDirEntryCheckedLong(tif,direntry,&m);
				*value=(float)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_SLONG:
			{
				int32 m;
				TIFFReadDirEntryCheckedSlong(tif,direntry,&m);
				*value=(float)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_LONG8:
			{
				uint64 m;
				err=TIFFReadDirEntryCheckedLong8(tif,direntry,&m);
				if (err!=TIFFReadDirEntryErrOk)
					return(err);
#if defined(__WIN32__) && (_MSC_VER < 1500)
				/*
				 * XXX: MSVC 6.0 does not support conversion
				 * of 64-bit integers into floating point
				 * values.
				 */
				*value = _TIFFUInt64ToFloat(m);
#else
				*value=(float)m;
#endif
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_SLONG8:
			{
				int64 m;
				err=TIFFReadDirEntryCheckedSlong8(tif,direntry,&m);
				if (err!=TIFFReadDirEntryErrOk)
					return(err);
				*value=(float)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_RATIONAL:
			{
				double m;
				err=TIFFReadDirEntryCheckedRational(tif,direntry,&m);
				if (err!=TIFFReadDirEntryErrOk)
					return(err);
				*value=(float)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_SRATIONAL:
			{
				double m;
				err=TIFFReadDirEntryCheckedSrational(tif,direntry,&m);
				if (err!=TIFFReadDirEntryErrOk)
					return(err);
				*value=(float)m;
				return(TIFFReadDirEntryErrOk);
			}
		case TIFF_FLOAT:
			TIFFReadDirEntryCheckedFloat(tif,direntry,value);
			return(TIFFReadDirEntryErrOk);
		case TIFF_DOUBLE:
			{
				double m;
				err=TIFFReadDirEntryCheckedDouble(tif,direntry,&m);
				if (err!=TIFFReadDirEntryErrOk)
					return(err);
				if ((m > FLT_MAX) || (m < FLT_MIN))
					return(TIFFReadDirEntryErrRange);
				*value=(float)m;
				return(TIFFReadDirEntryErrOk);
			}
		default:
			return(TIFFReadDirEntryErrType);
	}
}