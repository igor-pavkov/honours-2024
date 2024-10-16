inline int saturate(float fvalue, float minval, float maxval)
{
    if (fvalue > maxval) 
    {
        fvalue = maxval;
    } 
    else if (fvalue < minval)
    {
        fvalue = minval;
    }
    return (int)fvalue;
}