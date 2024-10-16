const char *am_get_header_attr(request_rec *r, const char *h,
                               const char *v, const char *a) 
{
    const char *value;
    const char *attr;
    char *l1;
    const char *attr_value = NULL;

    /* Looking for 
     * header-value; item_name="item_value"\n 
     */
    if ((value = am_xstrtok(r, h, ";", &l1)) == NULL)
        return NULL;
    am_strip_blank(&value);

    /* If a header value was provided, check it */ 
    if ((v != NULL) && (strcasecmp(value, v) != 0))
        return NULL;

    /* If no attribute name is provided, return everything */
    if (a == NULL)
        return h;

    while ((attr = am_xstrtok(r, NULL, ";", &l1)) != NULL) {
        const char *attr_name = NULL;
        char *l2;

        am_strip_blank(&attr);

        attr_name = am_xstrtok(r, attr, "=", &l2); 
        if ((attr_name != NULL) && (strcasecmp(attr_name, a) == 0)) {
            if ((attr_value = am_xstrtok(r, NULL, "=", &l2)) != NULL)
                am_strip_blank(&attr_value);
            break;
        }
    }
  
    /* Remove leading and trailing quotes */
    if (attr_value != NULL) {
        apr_size_t len; 

        len = strlen(attr_value);
        if ((len > 1) && (attr_value[len - 1] == '\"'))
            attr_value = apr_pstrndup(r->pool, attr_value, len - 1);
        if (attr_value[0] == '\"')
            attr_value++;
    }
    
    return attr_value;
}