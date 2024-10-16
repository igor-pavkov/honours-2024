static int vcf_parse_format(kstring_t *s, const bcf_hdr_t *h, bcf1_t *v, char *p, char *q)
{
    if ( !bcf_hdr_nsamples(h) ) return 0;

    static int extreme_val_warned = 0;
    char *r, *t;
    int j, l, m, g, overflow = 0;
    khint_t k;
    ks_tokaux_t aux1;
    vdict_t *d = (vdict_t*)h->dict[BCF_DT_ID];
    kstring_t *mem = (kstring_t*)&h->mem;
    fmt_aux_t fmt[MAX_N_FMT];
    mem->l = 0;

    char *end = s->s + s->l;
    if ( q>=end )
    {
        hts_log_error("FORMAT column with no sample columns starting at %s:%"PRIhts_pos"", bcf_seqname_safe(h,v), v->pos+1);
        v->errcode |= BCF_ERR_NCOLS;
        return -1;
    }

    v->n_fmt = 0;
    if ( p[0]=='.' && p[1]==0 ) // FORMAT field is empty "."
    {
        v->n_sample = bcf_hdr_nsamples(h);
        return 0;
    }

    // get format information from the dictionary
    for (j = 0, t = kstrtok(p, ":", &aux1); t; t = kstrtok(0, 0, &aux1), ++j) {
        if (j >= MAX_N_FMT) {
            v->errcode |= BCF_ERR_LIMITS;
            hts_log_error("FORMAT column at %s:%"PRIhts_pos" lists more identifiers than htslib can handle",
                bcf_seqname_safe(h,v), v->pos+1);
            return -1;
        }

        *(char*)aux1.p = 0;
        k = kh_get(vdict, d, t);
        if (k == kh_end(d) || kh_val(d, k).info[BCF_HL_FMT] == 15) {
            if ( t[0]=='.' && t[1]==0 )
            {
                hts_log_error("Invalid FORMAT tag name '.' at %s:%"PRIhts_pos, bcf_seqname_safe(h,v), v->pos+1);
                v->errcode |= BCF_ERR_TAG_INVALID;
                return -1;
            }
            hts_log_warning("FORMAT '%s' at %s:%"PRIhts_pos" is not defined in the header, assuming Type=String", t, bcf_seqname_safe(h,v), v->pos+1);
            kstring_t tmp = {0,0,0};
            int l;
            ksprintf(&tmp, "##FORMAT=<ID=%s,Number=1,Type=String,Description=\"Dummy\">", t);
            bcf_hrec_t *hrec = bcf_hdr_parse_line(h,tmp.s,&l);
            free(tmp.s);
            int res = hrec ? bcf_hdr_add_hrec((bcf_hdr_t*)h, hrec) : -1;
            if (res < 0) bcf_hrec_destroy(hrec);
            if (res > 0) res = bcf_hdr_sync((bcf_hdr_t*)h);

            k = kh_get(vdict, d, t);
            v->errcode = BCF_ERR_TAG_UNDEF;
            if (res || k == kh_end(d)) {
                hts_log_error("Could not add dummy header for FORMAT '%s' at %s:%"PRIhts_pos, t, bcf_seqname_safe(h,v), v->pos+1);
                v->errcode |= BCF_ERR_TAG_INVALID;
                return -1;
            }
        }
        fmt[j].max_l = fmt[j].max_m = fmt[j].max_g = 0;
        fmt[j].key = kh_val(d, k).id;
        fmt[j].is_gt = !strcmp(t, "GT");
        fmt[j].y = h->id[0][fmt[j].key].val->info[BCF_HL_FMT];
        v->n_fmt++;
    }
    // compute max
    int n_sample_ori = -1;
    r = q + 1;  // r: position in the format string
    l = 0, m = g = 1, v->n_sample = 0;  // m: max vector size, l: max field len, g: max number of alleles
    while ( r<end )
    {
        // can we skip some samples?
        if ( h->keep_samples )
        {
            n_sample_ori++;
            if ( !bit_array_test(h->keep_samples,n_sample_ori) )
            {
                while ( *r!='\t' && r<end ) r++;
                if ( *r=='\t' ) { *r = 0; r++; }
                continue;
            }
        }

        // collect fmt stats: max vector size, length, number of alleles
        j = 0;  // j-th format field
        fmt_aux_t *f = fmt;
        for (;;) {
            switch (*r) {
            case ',':
                m++;
                break;

            case '|':
            case '/':
                if (f->is_gt) g++;
                break;

            case '\t':
                *r = 0; // fall through

            case '\0':
            case ':':
                if (f->max_m < m) f->max_m = m;
                if (f->max_l < l) f->max_l = l;
                if (f->is_gt && f->max_g < g) f->max_g = g;
                l = 0, m = g = 1;
                if ( *r==':' ) {
                    j++; f++;
                    if ( j>=v->n_fmt ) {
                        hts_log_error("Incorrect number of FORMAT fields at %s:%"PRIhts_pos"",
                                      h->id[BCF_DT_CTG][v->rid].key, v->pos+1);
                        v->errcode |= BCF_ERR_NCOLS;
                        return -1;
                    }
                } else goto end_for;
                break;
            }
            if ( r>=end ) break;
            r++; l++;
        }
    end_for:
        v->n_sample++;
        if ( v->n_sample == bcf_hdr_nsamples(h) ) break;
        r++;
    }

    // allocate memory for arrays
    for (j = 0; j < v->n_fmt; ++j) {
        fmt_aux_t *f = &fmt[j];
        if ( !f->max_m ) f->max_m = 1;  // omitted trailing format field
        if ((f->y>>4&0xf) == BCF_HT_STR) {
            f->size = f->is_gt? f->max_g << 2 : f->max_l;
        } else if ((f->y>>4&0xf) == BCF_HT_REAL || (f->y>>4&0xf) == BCF_HT_INT) {
            f->size = f->max_m << 2;
        } else
        {
            hts_log_error("The format type %d at %s:%"PRIhts_pos" is currently not supported", f->y>>4&0xf, bcf_seqname_safe(h,v), v->pos+1);
            v->errcode |= BCF_ERR_TAG_INVALID;
            return -1;
        }
        if (align_mem(mem) < 0) {
            hts_log_error("Memory allocation failure at %s:%"PRIhts_pos, bcf_seqname_safe(h,v), v->pos+1);
            v->errcode |= BCF_ERR_LIMITS;
            return -1;
        }

        // Limit the total memory to ~2Gb per VCF row.  This should mean
        // malformed VCF data is less likely to take excessive memory and/or
        // time.
        if ((uint64_t) mem->l + v->n_sample * (uint64_t)f->size > INT_MAX) {
            hts_log_error("Excessive memory required by FORMAT fields at %s:%"PRIhts_pos, bcf_seqname_safe(h,v), v->pos+1);
            v->errcode |= BCF_ERR_LIMITS;
            return -1;
        }

        f->offset = mem->l;
        if (ks_resize(mem, mem->l + v->n_sample * (size_t)f->size) < 0) {
            hts_log_error("Memory allocation failure at %s:%"PRIhts_pos, bcf_seqname_safe(h,v), v->pos+1);
            v->errcode |= BCF_ERR_LIMITS;
            return -1;
        }
        mem->l += v->n_sample * f->size;
    }
    for (j = 0; j < v->n_fmt; ++j)
        fmt[j].buf = (uint8_t*)mem->s + fmt[j].offset;
    // fill the sample fields; at beginning of the loop, t points to the first char of a format
    n_sample_ori = -1;
    t = q + 1; m = 0;   // m: sample id
    while ( t<end )
    {
        // can we skip some samples?
        if ( h->keep_samples )
        {
            n_sample_ori++;
            if ( !bit_array_test(h->keep_samples,n_sample_ori) )
            {
                while ( *t && t<end ) t++;
                t++;
                continue;
            }
        }
        if ( m == bcf_hdr_nsamples(h) ) break;

        j = 0; // j-th format field, m-th sample
        while ( t < end )
        {
            fmt_aux_t *z = &fmt[j++];
            if (!z->buf) {
                hts_log_error("Memory allocation failure for FORMAT field type %d at %s:%"PRIhts_pos,
                              z->y>>4&0xf, bcf_seqname_safe(h,v), v->pos+1);
                v->errcode |= BCF_ERR_LIMITS;
                return -1;
            }
            if ((z->y>>4&0xf) == BCF_HT_STR) {
                if (z->is_gt) { // genotypes
                    int32_t is_phased = 0;
                    uint32_t *x = (uint32_t*)(z->buf + z->size * (size_t)m);
                    uint32_t unreadable = 0;
                    uint32_t max = 0;
                    overflow = 0;
                    for (l = 0;; ++t) {
                        if (*t == '.') {
                            ++t, x[l++] = is_phased;
                        } else {
                            char *tt = t;
                            uint32_t val = hts_str2uint(t, &t, sizeof(val) * CHAR_MAX - 2, &overflow);
                            unreadable |= tt == t;
                            if (max < val) max = val;
                            x[l++] = (val + 1) << 1 | is_phased;
                        }
                        is_phased = (*t == '|');
                        if (*t != '|' && *t != '/') break;
                    }
                    // Possibly check max against v->n_allele instead?
                    if (overflow || max > (INT32_MAX >> 1) - 1) {
                        hts_log_error("Couldn't read GT data: value too large at %s:%"PRIhts_pos, bcf_seqname_safe(h,v), v->pos+1);
                        return -1;
                    }
                    if (unreadable) {
                        hts_log_error("Couldn't read GT data: value not a number or '.' at %s:%"PRIhts_pos, bcf_seqname_safe(h,v), v->pos+1);
                        return -1;
                    }
                    if ( !l ) x[l++] = 0;   // An empty field, insert missing value
                    for (; l < z->size>>2; ++l) x[l] = bcf_int32_vector_end;
                } else {
                    char *x = (char*)z->buf + z->size * (size_t)m;
                    for (r = t, l = 0; *t != ':' && *t; ++t) x[l++] = *t;
                    for (; l < z->size; ++l) x[l] = 0;
                }
            } else if ((z->y>>4&0xf) == BCF_HT_INT) {
                int32_t *x = (int32_t*)(z->buf + z->size * (size_t)m);
                for (l = 0;; ++t) {
                    if (*t == '.') {
                        x[l++] = bcf_int32_missing, ++t; // ++t to skip "."
                    } else {
                        overflow = 0;
                        char *te;
                        long int tmp_val = hts_str2int(t, &te, sizeof(tmp_val)*CHAR_BIT, &overflow);
                        if ( te==t || overflow || tmp_val<BCF_MIN_BT_INT32 || tmp_val>BCF_MAX_BT_INT32 )
                        {
                            if ( !extreme_val_warned )
                            {
                                hts_log_warning("Extreme FORMAT/%s value encountered and set to missing at %s:%"PRIhts_pos, h->id[BCF_DT_ID][fmt[j-1].key].key, bcf_seqname_safe(h,v), v->pos+1);
                                extreme_val_warned = 1;
                            }
                            tmp_val = bcf_int32_missing;
                        }
                        x[l++] = tmp_val;
                        t = te;
                    }
                    if (*t != ',') break;
                }
                if ( !l ) x[l++] = bcf_int32_missing;
                for (; l < z->size>>2; ++l) x[l] = bcf_int32_vector_end;
            } else if ((z->y>>4&0xf) == BCF_HT_REAL) {
                float *x = (float*)(z->buf + z->size * (size_t)m);
                for (l = 0;; ++t) {
                    if (*t == '.' && !isdigit_c(t[1])) {
                        bcf_float_set_missing(x[l++]), ++t; // ++t to skip "."
                    } else {
                        overflow = 0;
                        char *te;
                        float tmp_val = hts_str2dbl(t, &te, &overflow);
                        if ( (te==t || overflow) && !extreme_val_warned )
                        {
                            hts_log_warning("Extreme FORMAT/%s value encountered at %s:%"PRIhts_pos, h->id[BCF_DT_ID][fmt[j-1].key].key, bcf_seqname(h,v), v->pos+1);
                            extreme_val_warned = 1;
                        }
                        x[l++] = tmp_val;
                        t = te;
                    }
                    if (*t != ',') break;
                }
                if ( !l ) bcf_float_set_missing(x[l++]);    // An empty field, insert missing value
                for (; l < z->size>>2; ++l) bcf_float_set_vector_end(x[l]);
            } else {
                hts_log_error("Unknown FORMAT field type %d at %s:%"PRIhts_pos, z->y>>4&0xf, bcf_seqname_safe(h,v), v->pos+1);
                v->errcode |= BCF_ERR_TAG_INVALID;
                return -1;
            }

            if (*t == '\0') {
                break;
            }
            else if (*t == ':') {
                t++;
            }
            else {
                char buffer[8];
                hts_log_error("Invalid character %s in '%s' FORMAT field at %s:%"PRIhts_pos"",
                    hts_strprint(buffer, sizeof buffer, '\'', t, 1),
                    h->id[BCF_DT_ID][z->key].key, bcf_seqname_safe(h,v), v->pos+1);
                v->errcode |= BCF_ERR_CHAR;
                return -1;
            }
        }

        for (; j < v->n_fmt; ++j) { // fill end-of-vector values
            fmt_aux_t *z = &fmt[j];
            if ((z->y>>4&0xf) == BCF_HT_STR) {
                if (z->is_gt) {
                    int32_t *x = (int32_t*)(z->buf + z->size * (size_t)m);
                    if (z->size) x[0] = bcf_int32_missing;
                    for (l = 1; l < z->size>>2; ++l) x[l] = bcf_int32_vector_end;
                } else {
                    char *x = (char*)z->buf + z->size * (size_t)m;
                    if ( z->size ) x[0] = '.';
                    for (l = 1; l < z->size; ++l) x[l] = 0;
                }
            } else if ((z->y>>4&0xf) == BCF_HT_INT) {
                int32_t *x = (int32_t*)(z->buf + z->size * (size_t)m);
                x[0] = bcf_int32_missing;
                for (l = 1; l < z->size>>2; ++l) x[l] = bcf_int32_vector_end;
            } else if ((z->y>>4&0xf) == BCF_HT_REAL) {
                float *x = (float*)(z->buf + z->size * (size_t)m);
                bcf_float_set_missing(x[0]);
                for (l = 1; l < z->size>>2; ++l) bcf_float_set_vector_end(x[l]);
            }
        }

        m++; t++;
    }

    // write individual genotype information
    kstring_t *str = &v->indiv;
    int i;
    if (v->n_sample > 0) {
        for (i = 0; i < v->n_fmt; ++i) {
            fmt_aux_t *z = &fmt[i];
            bcf_enc_int1(str, z->key);
            if ((z->y>>4&0xf) == BCF_HT_STR && !z->is_gt) {
                bcf_enc_size(str, z->size, BCF_BT_CHAR);
                kputsn((char*)z->buf, z->size * (size_t)v->n_sample, str);
            } else if ((z->y>>4&0xf) == BCF_HT_INT || z->is_gt) {
                bcf_enc_vint(str, (z->size>>2) * v->n_sample, (int32_t*)z->buf, z->size>>2);
            } else {
                bcf_enc_size(str, z->size>>2, BCF_BT_FLOAT);
                if (serialize_float_array(str, (z->size>>2) * (size_t)v->n_sample,
                                          (float *) z->buf) != 0) {
                    v->errcode |= BCF_ERR_LIMITS;
                    hts_log_error("Out of memory at %s:%"PRIhts_pos, bcf_seqname_safe(h,v), v->pos+1);
                    return -1;
                }
            }
        }
    }

    if ( v->n_sample!=bcf_hdr_nsamples(h) )
    {
        hts_log_error("Number of columns at %s:%"PRIhts_pos" does not match the number of samples (%d vs %d)",
            bcf_seqname_safe(h,v), v->pos+1, v->n_sample, bcf_hdr_nsamples(h));
        v->errcode |= BCF_ERR_NCOLS;
        return -1;
    }
    if ( v->indiv.l > 0xffffffff )
    {
        hts_log_error("The FORMAT at %s:%"PRIhts_pos" is too long", bcf_seqname_safe(h,v), v->pos+1);
        v->errcode |= BCF_ERR_LIMITS;

        // Error recovery: return -1 if this is a critical error or 0 if we want to ignore the FORMAT and proceed
        v->n_fmt = 0;
        return -1;
    }

    return 0;
}