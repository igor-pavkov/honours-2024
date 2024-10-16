void ASN1_TYPE_set(ASN1_TYPE *a, int type, void *value)
{
    if (a->value.ptr != NULL) {
        ASN1_TYPE **tmp_a = &a;
        ASN1_primitive_free((ASN1_VALUE **)tmp_a, NULL);
    }
    a->type = type;
    if (type == V_ASN1_BOOLEAN)
        a->value.boolean = value ? 0xff : 0;
    else
        a->value.ptr = value;
}