static int asn1_item_embed_d2i(ASN1_VALUE **pval, const unsigned char **in,
                               long len, const ASN1_ITEM *it,
                               int tag, int aclass, char opt, ASN1_TLC *ctx,
                               int depth)
{
    const ASN1_TEMPLATE *tt, *errtt = NULL;
    const ASN1_EXTERN_FUNCS *ef;
    const ASN1_AUX *aux = it->funcs;
    ASN1_aux_cb *asn1_cb;
    const unsigned char *p = NULL, *q;
    unsigned char oclass;
    char seq_eoc, seq_nolen, cst, isopt;
    long tmplen;
    int i;
    int otag;
    int ret = 0;
    ASN1_VALUE **pchptr;

    if (pval == NULL)
        return 0;
    if (aux && aux->asn1_cb)
        asn1_cb = aux->asn1_cb;
    else
        asn1_cb = 0;

    if (++depth > ASN1_MAX_CONSTRUCTED_NEST) {
        ERR_raise(ERR_LIB_ASN1, ASN1_R_NESTED_TOO_DEEP);
        goto err;
    }

    switch (it->itype) {
    case ASN1_ITYPE_PRIMITIVE:
        if (it->templates) {
            /*
             * tagging or OPTIONAL is currently illegal on an item template
             * because the flags can't get passed down. In practice this
             * isn't a problem: we include the relevant flags from the item
             * template in the template itself.
             */
            if ((tag != -1) || opt) {
                ERR_raise(ERR_LIB_ASN1,
                          ASN1_R_ILLEGAL_OPTIONS_ON_ITEM_TEMPLATE);
                goto err;
            }
            return asn1_template_ex_d2i(pval, in, len,
                                        it->templates, opt, ctx, depth);
        }
        return asn1_d2i_ex_primitive(pval, in, len, it,
                                     tag, aclass, opt, ctx);

    case ASN1_ITYPE_MSTRING:
        p = *in;
        /* Just read in tag and class */
        ret = asn1_check_tlen(NULL, &otag, &oclass, NULL, NULL,
                              &p, len, -1, 0, 1, ctx);
        if (!ret) {
            ERR_raise(ERR_LIB_ASN1, ERR_R_NESTED_ASN1_ERROR);
            goto err;
        }

        /* Must be UNIVERSAL class */
        if (oclass != V_ASN1_UNIVERSAL) {
            /* If OPTIONAL, assume this is OK */
            if (opt)
                return -1;
            ERR_raise(ERR_LIB_ASN1, ASN1_R_MSTRING_NOT_UNIVERSAL);
            goto err;
        }
        /* Check tag matches bit map */
        if (!(ASN1_tag2bit(otag) & it->utype)) {
            /* If OPTIONAL, assume this is OK */
            if (opt)
                return -1;
            ERR_raise(ERR_LIB_ASN1, ASN1_R_MSTRING_WRONG_TAG);
            goto err;
        }
        return asn1_d2i_ex_primitive(pval, in, len, it, otag, 0, 0, ctx);

    case ASN1_ITYPE_EXTERN:
        /* Use new style d2i */
        ef = it->funcs;
        return ef->asn1_ex_d2i(pval, in, len, it, tag, aclass, opt, ctx);

    case ASN1_ITYPE_CHOICE:
        if (asn1_cb && !asn1_cb(ASN1_OP_D2I_PRE, pval, it, NULL))
            goto auxerr;
        if (*pval) {
            /* Free up and zero CHOICE value if initialised */
            i = asn1_get_choice_selector(pval, it);
            if ((i >= 0) && (i < it->tcount)) {
                tt = it->templates + i;
                pchptr = asn1_get_field_ptr(pval, tt);
                asn1_template_free(pchptr, tt);
                asn1_set_choice_selector(pval, -1, it);
            }
        } else if (!ASN1_item_ex_new(pval, it)) {
            ERR_raise(ERR_LIB_ASN1, ERR_R_NESTED_ASN1_ERROR);
            goto err;
        }
        /* CHOICE type, try each possibility in turn */
        p = *in;
        for (i = 0, tt = it->templates; i < it->tcount; i++, tt++) {
            pchptr = asn1_get_field_ptr(pval, tt);
            /*
             * We mark field as OPTIONAL so its absence can be recognised.
             */
            ret = asn1_template_ex_d2i(pchptr, &p, len, tt, 1, ctx, depth);
            /* If field not present, try the next one */
            if (ret == -1)
                continue;
            /* If positive return, read OK, break loop */
            if (ret > 0)
                break;
            /*
             * Must be an ASN1 parsing error.
             * Free up any partial choice value
             */
            asn1_template_free(pchptr, tt);
            errtt = tt;
            ERR_raise(ERR_LIB_ASN1, ERR_R_NESTED_ASN1_ERROR);
            goto err;
        }

        /* Did we fall off the end without reading anything? */
        if (i == it->tcount) {
            /* If OPTIONAL, this is OK */
            if (opt) {
                /* Free and zero it */
                ASN1_item_ex_free(pval, it);
                return -1;
            }
            ERR_raise(ERR_LIB_ASN1, ASN1_R_NO_MATCHING_CHOICE_TYPE);
            goto err;
        }

        asn1_set_choice_selector(pval, i, it);

        if (asn1_cb && !asn1_cb(ASN1_OP_D2I_POST, pval, it, NULL))
            goto auxerr;
        *in = p;
        return 1;

    case ASN1_ITYPE_NDEF_SEQUENCE:
    case ASN1_ITYPE_SEQUENCE:
        p = *in;
        tmplen = len;

        /* If no IMPLICIT tagging set to SEQUENCE, UNIVERSAL */
        if (tag == -1) {
            tag = V_ASN1_SEQUENCE;
            aclass = V_ASN1_UNIVERSAL;
        }
        /* Get SEQUENCE length and update len, p */
        ret = asn1_check_tlen(&len, NULL, NULL, &seq_eoc, &cst,
                              &p, len, tag, aclass, opt, ctx);
        if (!ret) {
            ERR_raise(ERR_LIB_ASN1, ERR_R_NESTED_ASN1_ERROR);
            goto err;
        } else if (ret == -1)
            return -1;
        if (aux && (aux->flags & ASN1_AFLG_BROKEN)) {
            len = tmplen - (p - *in);
            seq_nolen = 1;
        }
        /* If indefinite we don't do a length check */
        else
            seq_nolen = seq_eoc;
        if (!cst) {
            ERR_raise(ERR_LIB_ASN1, ASN1_R_SEQUENCE_NOT_CONSTRUCTED);
            goto err;
        }

        if (*pval == NULL && !ASN1_item_ex_new(pval, it)) {
            ERR_raise(ERR_LIB_ASN1, ERR_R_NESTED_ASN1_ERROR);
            goto err;
        }

        if (asn1_cb && !asn1_cb(ASN1_OP_D2I_PRE, pval, it, NULL))
            goto auxerr;

        /* Free up and zero any ADB found */
        for (i = 0, tt = it->templates; i < it->tcount; i++, tt++) {
            if (tt->flags & ASN1_TFLG_ADB_MASK) {
                const ASN1_TEMPLATE *seqtt;
                ASN1_VALUE **pseqval;
                seqtt = asn1_do_adb(*pval, tt, 0);
                if (seqtt == NULL)
                    continue;
                pseqval = asn1_get_field_ptr(pval, seqtt);
                asn1_template_free(pseqval, seqtt);
            }
        }

        /* Get each field entry */
        for (i = 0, tt = it->templates; i < it->tcount; i++, tt++) {
            const ASN1_TEMPLATE *seqtt;
            ASN1_VALUE **pseqval;
            seqtt = asn1_do_adb(*pval, tt, 1);
            if (seqtt == NULL)
                goto err;
            pseqval = asn1_get_field_ptr(pval, seqtt);
            /* Have we ran out of data? */
            if (!len)
                break;
            q = p;
            if (asn1_check_eoc(&p, len)) {
                if (!seq_eoc) {
                    ERR_raise(ERR_LIB_ASN1, ASN1_R_UNEXPECTED_EOC);
                    goto err;
                }
                len -= p - q;
                seq_eoc = 0;
                break;
            }
            /*
             * This determines the OPTIONAL flag value. The field cannot be
             * omitted if it is the last of a SEQUENCE and there is still
             * data to be read. This isn't strictly necessary but it
             * increases efficiency in some cases.
             */
            if (i == (it->tcount - 1))
                isopt = 0;
            else
                isopt = (char)(seqtt->flags & ASN1_TFLG_OPTIONAL);
            /*
             * attempt to read in field, allowing each to be OPTIONAL
             */

            ret = asn1_template_ex_d2i(pseqval, &p, len, seqtt, isopt, ctx,
                                       depth);
            if (!ret) {
                errtt = seqtt;
                goto err;
            } else if (ret == -1) {
                /*
                 * OPTIONAL component absent. Free and zero the field.
                 */
                asn1_template_free(pseqval, seqtt);
                continue;
            }
            /* Update length */
            len -= p - q;
        }

        /* Check for EOC if expecting one */
        if (seq_eoc && !asn1_check_eoc(&p, len)) {
            ERR_raise(ERR_LIB_ASN1, ASN1_R_MISSING_EOC);
            goto err;
        }
        /* Check all data read */
        if (!seq_nolen && len) {
            ERR_raise(ERR_LIB_ASN1, ASN1_R_SEQUENCE_LENGTH_MISMATCH);
            goto err;
        }

        /*
         * If we get here we've got no more data in the SEQUENCE, however we
         * may not have read all fields so check all remaining are OPTIONAL
         * and clear any that are.
         */
        for (; i < it->tcount; tt++, i++) {
            const ASN1_TEMPLATE *seqtt;
            seqtt = asn1_do_adb(*pval, tt, 1);
            if (seqtt == NULL)
                goto err;
            if (seqtt->flags & ASN1_TFLG_OPTIONAL) {
                ASN1_VALUE **pseqval;
                pseqval = asn1_get_field_ptr(pval, seqtt);
                asn1_template_free(pseqval, seqtt);
            } else {
                errtt = seqtt;
                ERR_raise(ERR_LIB_ASN1, ASN1_R_FIELD_MISSING);
                goto err;
            }
        }
        /* Save encoding */
        if (!asn1_enc_save(pval, *in, p - *in, it))
            goto auxerr;
        if (asn1_cb && !asn1_cb(ASN1_OP_D2I_POST, pval, it, NULL))
            goto auxerr;
        *in = p;
        return 1;

    default:
        return 0;
    }
 auxerr:
    ERR_raise(ERR_LIB_ASN1, ASN1_R_AUX_ERROR);
 err:
    if (errtt)
        ERR_add_error_data(4, "Field=", errtt->field_name,
                           ", Type=", it->sname);
    else
        ERR_add_error_data(2, "Type=", it->sname);
    return 0;
}