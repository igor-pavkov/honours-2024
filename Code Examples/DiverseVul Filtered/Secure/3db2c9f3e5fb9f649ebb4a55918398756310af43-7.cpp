static int asn1_template_ex_i2d(const ASN1_VALUE **pval, unsigned char **out,
                                const ASN1_TEMPLATE *tt, int tag, int iclass)
{
    int i, ret, flags, ttag, tclass, ndef;
    const ASN1_VALUE *tval;
    flags = tt->flags;

    /*
     * If field is embedded then val needs fixing so it is a pointer to
     * a pointer to a field.
     */
    if (flags & ASN1_TFLG_EMBED) {
        tval = (ASN1_VALUE *)pval;
        pval = &tval;
    }
    /*
     * Work out tag and class to use: tagging may come either from the
     * template or the arguments, not both because this would create
     * ambiguity. Additionally the iclass argument may contain some
     * additional flags which should be noted and passed down to other
     * levels.
     */
    if (flags & ASN1_TFLG_TAG_MASK) {
        /* Error if argument and template tagging */
        if (tag != -1)
            /* FIXME: error code here */
            return -1;
        /* Get tagging from template */
        ttag = tt->tag;
        tclass = flags & ASN1_TFLG_TAG_CLASS;
    } else if (tag != -1) {
        /* No template tagging, get from arguments */
        ttag = tag;
        tclass = iclass & ASN1_TFLG_TAG_CLASS;
    } else {
        ttag = -1;
        tclass = 0;
    }
    /*
     * Remove any class mask from iflag.
     */
    iclass &= ~ASN1_TFLG_TAG_CLASS;

    /*
     * At this point 'ttag' contains the outer tag to use, 'tclass' is the
     * class and iclass is any flags passed to this function.
     */

    /* if template and arguments require ndef, use it */
    if ((flags & ASN1_TFLG_NDEF) && (iclass & ASN1_TFLG_NDEF))
        ndef = 2;
    else
        ndef = 1;

    if (flags & ASN1_TFLG_SK_MASK) {
        /* SET OF, SEQUENCE OF */
        STACK_OF(const_ASN1_VALUE) *sk = (STACK_OF(const_ASN1_VALUE) *)*pval;
        int isset, sktag, skaclass;
        int skcontlen, sklen;
        const ASN1_VALUE *skitem;

        if (*pval == NULL)
            return 0;

        if (flags & ASN1_TFLG_SET_OF) {
            isset = 1;
            /* 2 means we reorder */
            if (flags & ASN1_TFLG_SEQUENCE_OF)
                isset = 2;
        } else
            isset = 0;

        /*
         * Work out inner tag value: if EXPLICIT or no tagging use underlying
         * type.
         */
        if ((ttag != -1) && !(flags & ASN1_TFLG_EXPTAG)) {
            sktag = ttag;
            skaclass = tclass;
        } else {
            skaclass = V_ASN1_UNIVERSAL;
            if (isset)
                sktag = V_ASN1_SET;
            else
                sktag = V_ASN1_SEQUENCE;
        }

        /* Determine total length of items */
        skcontlen = 0;
        for (i = 0; i < sk_const_ASN1_VALUE_num(sk); i++) {
            int tmplen;
            skitem = sk_const_ASN1_VALUE_value(sk, i);
            tmplen = ASN1_item_ex_i2d(&skitem, NULL, ASN1_ITEM_ptr(tt->item),
                                      -1, iclass);
            if (tmplen == -1 || (skcontlen > INT_MAX - tmplen))
                return -1;
            skcontlen += tmplen;
        }
        sklen = ASN1_object_size(ndef, skcontlen, sktag);
        if (sklen == -1)
            return -1;
        /* If EXPLICIT need length of surrounding tag */
        if (flags & ASN1_TFLG_EXPTAG)
            ret = ASN1_object_size(ndef, sklen, ttag);
        else
            ret = sklen;

        if (!out || ret == -1)
            return ret;

        /* Now encode this lot... */
        /* EXPLICIT tag */
        if (flags & ASN1_TFLG_EXPTAG)
            ASN1_put_object(out, ndef, sklen, ttag, tclass);
        /* SET or SEQUENCE and IMPLICIT tag */
        ASN1_put_object(out, ndef, skcontlen, sktag, skaclass);
        /* And the stuff itself */
        asn1_set_seq_out(sk, out, skcontlen, ASN1_ITEM_ptr(tt->item),
                         isset, iclass);
        if (ndef == 2) {
            ASN1_put_eoc(out);
            if (flags & ASN1_TFLG_EXPTAG)
                ASN1_put_eoc(out);
        }

        return ret;
    }

    if (flags & ASN1_TFLG_EXPTAG) {
        /* EXPLICIT tagging */
        /* Find length of tagged item */
        i = ASN1_item_ex_i2d(pval, NULL, ASN1_ITEM_ptr(tt->item), -1, iclass);
        if (!i)
            return 0;
        /* Find length of EXPLICIT tag */
        ret = ASN1_object_size(ndef, i, ttag);
        if (out && ret != -1) {
            /* Output tag and item */
            ASN1_put_object(out, ndef, i, ttag, tclass);
            ASN1_item_ex_i2d(pval, out, ASN1_ITEM_ptr(tt->item), -1, iclass);
            if (ndef == 2)
                ASN1_put_eoc(out);
        }
        return ret;
    }

    /* Either normal or IMPLICIT tagging: combine class and flags */
    return ASN1_item_ex_i2d(pval, out, ASN1_ITEM_ptr(tt->item),
                            ttag, tclass | iclass);

}