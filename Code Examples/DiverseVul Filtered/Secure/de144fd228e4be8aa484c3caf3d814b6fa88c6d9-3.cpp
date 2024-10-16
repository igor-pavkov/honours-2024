static int16 TIFFClampDoubleToInt16( double val )
{
    if( val > 32767 )
        return 32767;
    if( val < -32768 || val != val )
        return -32768;
    return (int16)val;
}