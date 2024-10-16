static uint16 TIFFClampDoubleToUInt16( double val )
{
    if( val < 0 )
        return 0;
    if( val > 65535 || val != val )
        return 65535;
    return (uint16)val;
}