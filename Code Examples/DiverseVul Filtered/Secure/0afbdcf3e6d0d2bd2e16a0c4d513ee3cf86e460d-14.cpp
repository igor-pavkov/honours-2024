static opj_image_t* convert_gray_to_rgb(opj_image_t* original)
{
    OPJ_UINT32 compno;
    opj_image_t* l_new_image = NULL;
    opj_image_cmptparm_t* l_new_components = NULL;

    l_new_components = (opj_image_cmptparm_t*)malloc((original->numcomps + 2U) *
                       sizeof(opj_image_cmptparm_t));
    if (l_new_components == NULL) {
        fprintf(stderr,
                "ERROR -> opj_decompress: failed to allocate memory for RGB image!\n");
        opj_image_destroy(original);
        return NULL;
    }

    l_new_components[0].bpp  = l_new_components[1].bpp  = l_new_components[2].bpp  =
                                   original->comps[0].bpp;
    l_new_components[0].dx   = l_new_components[1].dx   = l_new_components[2].dx   =
                                   original->comps[0].dx;
    l_new_components[0].dy   = l_new_components[1].dy   = l_new_components[2].dy   =
                                   original->comps[0].dy;
    l_new_components[0].h    = l_new_components[1].h    = l_new_components[2].h    =
                                   original->comps[0].h;
    l_new_components[0].w    = l_new_components[1].w    = l_new_components[2].w    =
                                   original->comps[0].w;
    l_new_components[0].prec = l_new_components[1].prec = l_new_components[2].prec =
                                   original->comps[0].prec;
    l_new_components[0].sgnd = l_new_components[1].sgnd = l_new_components[2].sgnd =
                                   original->comps[0].sgnd;
    l_new_components[0].x0   = l_new_components[1].x0   = l_new_components[2].x0   =
                                   original->comps[0].x0;
    l_new_components[0].y0   = l_new_components[1].y0   = l_new_components[2].y0   =
                                   original->comps[0].y0;

    for (compno = 1U; compno < original->numcomps; ++compno) {
        l_new_components[compno + 2U].bpp  = original->comps[compno].bpp;
        l_new_components[compno + 2U].dx   = original->comps[compno].dx;
        l_new_components[compno + 2U].dy   = original->comps[compno].dy;
        l_new_components[compno + 2U].h    = original->comps[compno].h;
        l_new_components[compno + 2U].w    = original->comps[compno].w;
        l_new_components[compno + 2U].prec = original->comps[compno].prec;
        l_new_components[compno + 2U].sgnd = original->comps[compno].sgnd;
        l_new_components[compno + 2U].x0   = original->comps[compno].x0;
        l_new_components[compno + 2U].y0   = original->comps[compno].y0;
    }

    l_new_image = opj_image_create(original->numcomps + 2U, l_new_components,
                                   OPJ_CLRSPC_SRGB);
    free(l_new_components);
    if (l_new_image == NULL) {
        fprintf(stderr,
                "ERROR -> opj_decompress: failed to allocate memory for RGB image!\n");
        opj_image_destroy(original);
        return NULL;
    }

    l_new_image->x0 = original->x0;
    l_new_image->x1 = original->x1;
    l_new_image->y0 = original->y0;
    l_new_image->y1 = original->y1;

    l_new_image->comps[0].factor        = l_new_image->comps[1].factor        =
            l_new_image->comps[2].factor        = original->comps[0].factor;
    l_new_image->comps[0].alpha         = l_new_image->comps[1].alpha         =
            l_new_image->comps[2].alpha         = original->comps[0].alpha;
    l_new_image->comps[0].resno_decoded = l_new_image->comps[1].resno_decoded =
            l_new_image->comps[2].resno_decoded = original->comps[0].resno_decoded;

    memcpy(l_new_image->comps[0].data, original->comps[0].data,
           sizeof(OPJ_INT32) * original->comps[0].w * original->comps[0].h);
    memcpy(l_new_image->comps[1].data, original->comps[0].data,
           sizeof(OPJ_INT32) * original->comps[0].w * original->comps[0].h);
    memcpy(l_new_image->comps[2].data, original->comps[0].data,
           sizeof(OPJ_INT32) * original->comps[0].w * original->comps[0].h);

    for (compno = 1U; compno < original->numcomps; ++compno) {
        l_new_image->comps[compno + 2U].factor        = original->comps[compno].factor;
        l_new_image->comps[compno + 2U].alpha         = original->comps[compno].alpha;
        l_new_image->comps[compno + 2U].resno_decoded =
            original->comps[compno].resno_decoded;
        memcpy(l_new_image->comps[compno + 2U].data, original->comps[compno].data,
               sizeof(OPJ_INT32) * original->comps[compno].w * original->comps[compno].h);
    }
    opj_image_destroy(original);
    return l_new_image;
}