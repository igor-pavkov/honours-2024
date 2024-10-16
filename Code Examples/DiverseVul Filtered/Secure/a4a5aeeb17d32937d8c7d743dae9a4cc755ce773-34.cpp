bool CWebSock::ForceLogin() {
    if (GetSession()->IsLoggedIn()) {
        return true;
    }

    GetSession()->AddError("You must login to view that page");
    Redirect("/");
    return false;
}