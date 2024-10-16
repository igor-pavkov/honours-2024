static int ntop_get_mac_manufacturer(lua_State* vm) {
  const char *mac = NULL;

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  mac = (char*)lua_tostring(vm, 1);

  ntop->getMacManufacturer(mac, vm);
  return(CONST_LUA_OK);
}