static uint32 TIFFClampDoubleToUInt32( double val )
{
    if( val < 0 )
        return 0;
    if( val > 0xFFFFFFFFU || val != val )
        return 0xFFFFFFFFU;
    return (uint32)val;
}