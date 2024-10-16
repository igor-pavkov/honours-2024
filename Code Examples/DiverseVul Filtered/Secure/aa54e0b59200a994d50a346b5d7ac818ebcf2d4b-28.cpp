FriBidiParType resolve_base_direction(int enc)
{
    switch (enc) {
        case -1:
            return FRIBIDI_PAR_ON;
        default:
            return FRIBIDI_PAR_LTR;
    }
}