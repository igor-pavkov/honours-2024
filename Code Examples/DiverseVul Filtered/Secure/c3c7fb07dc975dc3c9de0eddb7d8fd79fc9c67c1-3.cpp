int ASN1_TYPE_get(ASN1_TYPE *a)
{
    if ((a->value.ptr != NULL) || (a->type == V_ASN1_NULL))
        return (a->type);
    else
        return (0);
}