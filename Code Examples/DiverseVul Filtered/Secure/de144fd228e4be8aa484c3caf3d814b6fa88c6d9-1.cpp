static int8 TIFFClampDoubleToInt8( double val )
{
    if( val > 127 )
        return 127;
    if( val < -128 || val != val )
        return -128;
    return (int8)val;
}