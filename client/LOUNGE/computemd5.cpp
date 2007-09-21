#include <stdio.h>

int ComputeMD5(const char* fname)
{
  FILE* f = fopen(fname, "r");
  if (f == NULL)
    return -1;

  if (fseek(f, 0, SEEK_END) == 0)
  {
    long size = ftell(f);
    fseek(f, 0, SEEK_BEGIN);

    char* buf = new char[size];
    if (buf != 0)
    {
      long ofs = 0,
           chunk = 4096;

      while (size > 0)
      {
        long nread = fread(&buf[ofs], 1, 4096, f);
        
        if (nread == 0)
          break;

        ofs += nread;
        size -= nread;
      }

      fclose(f);
      f = NULL;

      if (size == 0)
      {

      }
    }
  }
}

int main()
{
}