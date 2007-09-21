// Old drawChips:
#ifdef OLD_CODE_
BOOL Chips::drawChips(CDC* pDC, USHORT chips, int xpos, int ypos, DWORD dwFlags)
{
  if (chips == 0)
    return TRUE;

  int n = getNumChipUnits(chips);

  CDC memDC, bufDC, transDC;

  BOOL rc = bufDC.CreateCompatibleDC(pDC) &&
            memDC.CreateCompatibleDC(pDC) &&
            transDC.CreateCompatibleDC(pDC);


/*
  // First blt thousands
  if (chips >= 1000)
  {
    int thousands = chips / 1000U;
    if (thousands > 0)
    {
      // NOT IMPL
    }
    xpos += Width_;
    chips -= (thousands * 1000);
  }
*/
  if (chips >= 500)
  {
    int fh = chips / 500U;
    int next_y = ypos;
    bltChips(pDC, fh * 500,
             500, &fiveHundredChip_,
             xpos, ypos,
             Width_, Height_,
             next_y,
             bufDC, memDC, transDC);
    xpos += Width_;
    chips -= (fh * 500);
  }

  if (chips >= 100)
  {
    int hundreds = chips / 100U;
    int next_y = ypos;
    bltChips(pDC, hundreds * 100,
             100, &hundredChip_,
             xpos, ypos,
             Width_, Height_,
             next_y,
             bufDC, memDC, transDC);
    xpos += Width_;
    chips -= (hundreds * 100);
  }

  if (chips >= 25)
  {
    int c = chips / 25U;
    int next_y = ypos;
    bltChips(pDC, c * 25,
             25, &twenty5Chip_,
             xpos, ypos,
             Width_, Height_,
             next_y,
             bufDC, memDC, transDC);
    xpos += Width_;
    chips -= (c * 25);
  }

  if (chips >= 10)
  {
    int tens = chips / 10U;
    int next_y = ypos;
    bltChips(pDC, tens * 10,
             5, &fiveChip_,
             xpos, ypos,
             Width_, Height_,
             next_y,
             bufDC, memDC, transDC);
    xpos += Width_;
    chips -= (tens * 10);
  }

  if (chips > 0)
  {
    int next_y = ypos;

    if (chips >= 5)
    {
      bltChips(pDC, chips,
               5, &fiveChip_,
               xpos, ypos,
               Width_, Height_,
               next_y,
               bufDC, memDC, transDC);
      chips -= 5;
    }

    bltChips(pDC, chips,
             1, &oneChip_,
             xpos, next_y,
             Width_, Height_,
             next_y,
             bufDC, memDC, transDC);
  }

  return rc;
}
#endif
