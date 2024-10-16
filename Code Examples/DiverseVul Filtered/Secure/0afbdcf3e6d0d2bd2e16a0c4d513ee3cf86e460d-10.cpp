static opj_image_t* upsample_image_components(opj_image_t* original)
{
    opj_image_t* l_new_image = NULL;
    opj_image_cmptparm_t* l_new_components = NULL;
    OPJ_BOOL l_upsample_need = OPJ_FALSE;
    OPJ_UINT32 compno;

    for (compno = 0U; compno < original->numcomps; ++compno) {
        if (original->comps[compno].factor > 0U) {
            fprintf(stderr,
                    "ERROR -> opj_decompress: -upsample not supported with reduction\n");
            opj_image_destroy(original);
            return NULL;
        }
        if ((original->comps[compno].dx > 1U) || (original->comps[compno].dy > 1U)) {
            l_upsample_need = OPJ_TRUE;
            break;
        }
    }
    if (!l_upsample_need) {
        return original;
    }
    /* Upsample is needed */
    l_new_components = (opj_image_cmptparm_t*)malloc(original->numcomps * sizeof(
                           opj_image_cmptparm_t));
    if (l_new_components == NULL) {
        fprintf(stderr,
                "ERROR -> opj_decompress: failed to allocate memory for upsampled components!\n");
        opj_image_destroy(original);
        return NULL;
    }

    for (compno = 0U; compno < original->numcomps; ++compno) {
        opj_image_cmptparm_t* l_new_cmp = &(l_new_components[compno]);
        opj_image_comp_t*     l_org_cmp = &(original->comps[compno]);

        l_new_cmp->bpp  = l_org_cmp->bpp;
        l_new_cmp->prec = l_org_cmp->prec;
        l_new_cmp->sgnd = l_org_cmp->sgnd;
        l_new_cmp->x0   = original->x0;
        l_new_cmp->y0   = original->y0;
        l_new_cmp->dx   = 1;
        l_new_cmp->dy   = 1;
        l_new_cmp->w    =
            l_org_cmp->w; /* should be original->x1 - original->x0 for dx==1 */
        l_new_cmp->h    =
            l_org_cmp->h; /* should be original->y1 - original->y0 for dy==0 */

        if (l_org_cmp->dx > 1U) {
            l_new_cmp->w = original->x1 - original->x0;
        }

        if (l_org_cmp->dy > 1U) {
            l_new_cmp->h = original->y1 - original->y0;
        }
    }

    l_new_image = opj_image_create(original->numcomps, l_new_components,
                                   original->color_space);
    free(l_new_components);
    if (l_new_image == NULL) {
        fprintf(stderr,
                "ERROR -> opj_decompress: failed to allocate memory for upsampled components!\n");
        opj_image_destroy(original);
        return NULL;
    }

    l_new_image->x0 = original->x0;
    l_new_image->x1 = original->x1;
    l_new_image->y0 = original->y0;
    l_new_image->y1 = original->y1;

    for (compno = 0U; compno < original->numcomps; ++compno) {
        opj_image_comp_t* l_new_cmp = &(l_new_image->comps[compno]);
        opj_image_comp_t* l_org_cmp = &(original->comps[compno]);

        l_new_cmp->factor        = l_org_cmp->factor;
        l_new_cmp->alpha         = l_org_cmp->alpha;
        l_new_cmp->resno_decoded = l_org_cmp->resno_decoded;

        if ((l_org_cmp->dx > 1U) || (l_org_cmp->dy > 1U)) {
            const OPJ_INT32* l_src = l_org_cmp->data;
            OPJ_INT32*       l_dst = l_new_cmp->data;
            OPJ_UINT32 y;
            OPJ_UINT32 xoff, yoff;

            /* need to take into account dx & dy */
            xoff = l_org_cmp->dx * l_org_cmp->x0 -  original->x0;
            yoff = l_org_cmp->dy * l_org_cmp->y0 -  original->y0;
            if ((xoff >= l_org_cmp->dx) || (yoff >= l_org_cmp->dy)) {
                fprintf(stderr,
                        "ERROR -> opj_decompress: Invalid image/component parameters found when upsampling\n");
                opj_image_destroy(original);
                opj_image_destroy(l_new_image);
                return NULL;
            }

            for (y = 0U; y < yoff; ++y) {
                memset(l_dst, 0U, l_new_cmp->w * sizeof(OPJ_INT32));
                l_dst += l_new_cmp->w;
            }

            if (l_new_cmp->h > (l_org_cmp->dy -
                                1U)) { /* check subtraction overflow for really small images */
                for (; y < l_new_cmp->h - (l_org_cmp->dy - 1U); y += l_org_cmp->dy) {
                    OPJ_UINT32 x, dy;
                    OPJ_UINT32 xorg;

                    xorg = 0U;
                    for (x = 0U; x < xoff; ++x) {
                        l_dst[x] = 0;
                    }
                    if (l_new_cmp->w > (l_org_cmp->dx -
                                        1U)) { /* check subtraction overflow for really small images */
                        for (; x < l_new_cmp->w - (l_org_cmp->dx - 1U); x += l_org_cmp->dx, ++xorg) {
                            OPJ_UINT32 dx;
                            for (dx = 0U; dx < l_org_cmp->dx; ++dx) {
                                l_dst[x + dx] = l_src[xorg];
                            }
                        }
                    }
                    for (; x < l_new_cmp->w; ++x) {
                        l_dst[x] = l_src[xorg];
                    }
                    l_dst += l_new_cmp->w;

                    for (dy = 1U; dy < l_org_cmp->dy; ++dy) {
                        memcpy(l_dst, l_dst - l_new_cmp->w, l_new_cmp->w * sizeof(OPJ_INT32));
                        l_dst += l_new_cmp->w;
                    }
                    l_src += l_org_cmp->w;
                }
            }
            if (y < l_new_cmp->h) {
                OPJ_UINT32 x;
                OPJ_UINT32 xorg;

                xorg = 0U;
                for (x = 0U; x < xoff; ++x) {
                    l_dst[x] = 0;
                }
                if (l_new_cmp->w > (l_org_cmp->dx -
                                    1U)) { /* check subtraction overflow for really small images */
                    for (; x < l_new_cmp->w - (l_org_cmp->dx - 1U); x += l_org_cmp->dx, ++xorg) {
                        OPJ_UINT32 dx;
                        for (dx = 0U; dx < l_org_cmp->dx; ++dx) {
                            l_dst[x + dx] = l_src[xorg];
                        }
                    }
                }
                for (; x < l_new_cmp->w; ++x) {
                    l_dst[x] = l_src[xorg];
                }
                l_dst += l_new_cmp->w;
                ++y;
                for (; y < l_new_cmp->h; ++y) {
                    memcpy(l_dst, l_dst - l_new_cmp->w, l_new_cmp->w * sizeof(OPJ_INT32));
                    l_dst += l_new_cmp->w;
                }
            }
        } else {
            memcpy(l_new_cmp->data, l_org_cmp->data,
                   sizeof(OPJ_INT32) * l_org_cmp->w * l_org_cmp->h);
        }
    }
    opj_image_destroy(original);
    return l_new_image;
}