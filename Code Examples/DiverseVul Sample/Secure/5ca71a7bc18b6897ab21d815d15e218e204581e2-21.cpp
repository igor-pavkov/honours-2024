cmsBool  _cmsRegisterTagTypePlugin(cmsContext id, cmsPluginBase* Data)
{
    return RegisterTypesPlugin(id, Data, TagTypePlugin);
}