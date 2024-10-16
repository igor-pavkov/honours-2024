static MngBox mng_minimum_box(MngBox box1,MngBox box2)
{
  MngBox
    box;

  box=box1;
  if (box.left < box2.left)
    box.left=box2.left;

  if (box.top < box2.top)
    box.top=box2.top;

  if (box.right > box2.right)
    box.right=box2.right;

  if (box.bottom > box2.bottom)
    box.bottom=box2.bottom;

  return box;
}