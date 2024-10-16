virNodeDevCapMdevAttributeParseXML(xmlXPathContextPtr ctxt,
                                   xmlNodePtr node,
                                   virNodeDevCapMdevPtr mdev)
{
    VIR_XPATH_NODE_AUTORESTORE(ctxt)
    g_autoptr(virMediatedDeviceAttr) attr = virMediatedDeviceAttrNew();

    ctxt->node = node;
    attr->name = virXPathString("string(./@name)", ctxt);
    attr->value = virXPathString("string(./@value)", ctxt);
    if (!attr->name || !attr->value) {
        virReportError(VIR_ERR_CONFIG_UNSUPPORTED, "%s",
                       _("mdev attribute missing name or value"));
        return -1;
    }

    return VIR_APPEND_ELEMENT(mdev->attributes,
                              mdev->nattributes,
                              attr);
}