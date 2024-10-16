unsigned long ASN1_tag2bit(int tag)
{
    if ((tag < 0) || (tag > 30))
        return 0;
    return tag2bit[tag];
}