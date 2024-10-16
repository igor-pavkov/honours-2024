static float TIFFClampDoubleToFloat( double val )
{
    if( val > FLT_MAX )
        return FLT_MAX;
    if( val < -FLT_MAX )
        return -FLT_MAX;
    return (float)val;
}