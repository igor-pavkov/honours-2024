static int ntop_get_site_categories(lua_State* vm) {
  Flashstart *flash = ntop->get_flashstart();

  if (!flash)
    lua_pushnil(vm);
  else
    flash->lua(vm);

  return(CONST_LUA_OK);
}