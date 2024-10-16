static int32 TIFFClampDoubleToInt32( double val )
{
    if( val > 0x7FFFFFFF )
        return 0x7FFFFFFF;
    if( val < -0x7FFFFFFF-1 || val != val )
        return -0x7FFFFFFF-1;
    return (int32)val;
}