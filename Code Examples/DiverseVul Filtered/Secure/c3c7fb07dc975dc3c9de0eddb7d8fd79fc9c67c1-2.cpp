int ASN1_TYPE_set1(ASN1_TYPE *a, int type, const void *value)
{
    if (!value || (type == V_ASN1_BOOLEAN)) {
        void *p = (void *)value;
        ASN1_TYPE_set(a, type, p);
    } else if (type == V_ASN1_OBJECT) {
        ASN1_OBJECT *odup;
        odup = OBJ_dup(value);
        if (!odup)
            return 0;
        ASN1_TYPE_set(a, type, odup);
    } else {
        ASN1_STRING *sdup;
        sdup = ASN1_STRING_dup(value);
        if (!sdup)
            return 0;
        ASN1_TYPE_set(a, type, sdup);
    }
    return 1;
}