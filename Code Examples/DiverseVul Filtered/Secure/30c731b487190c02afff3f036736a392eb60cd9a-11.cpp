int HtmlOutputDev::getOutlinePageNum(OutlineItem *item)
{
    const LinkAction *action   = item->getAction();
    const LinkGoTo   *link     = nullptr;
    LinkDest   *linkdest = nullptr;
    int         pagenum  = -1;

    if (!action || action->getKind() != actionGoTo)
        return pagenum;

    link = dynamic_cast<const LinkGoTo*>(action);

    if (!link || !link->isOk())
        return pagenum;

    if (link->getDest())
        linkdest = link->getDest()->copy();
    else if (link->getNamedDest())
        linkdest = catalog->findDest(link->getNamedDest());

    if (!linkdest)
        return pagenum;

    if (linkdest->isPageRef()) {
        Ref pageref = linkdest->getPageRef();
        pagenum = catalog->findPage(pageref.num, pageref.gen);
    } else {
        pagenum = linkdest->getPageNum();
    }

    delete linkdest;
    return pagenum;
}