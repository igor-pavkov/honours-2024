static uint8 TIFFClampDoubleToUInt8( double val )
{
    if( val < 0 )
        return 0;
    if( val > 255 || val != val )
        return 255;
    return (uint8)val;
}