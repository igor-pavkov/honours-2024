int ASN1_item_ndef_i2d(const ASN1_VALUE *val, unsigned char **out,
                       const ASN1_ITEM *it)
{
    return asn1_item_flags_i2d(val, out, it, ASN1_TFLG_NDEF);
}