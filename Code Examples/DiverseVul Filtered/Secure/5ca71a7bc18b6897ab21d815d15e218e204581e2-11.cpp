cmsBool  _cmsRegisterMultiProcessElementPlugin(cmsContext id, cmsPluginBase* Data)
{
    return RegisterTypesPlugin(id, Data,MPEPlugin);
}