#include "random_generator.h"
#include "sys.h"
#include <stdlib.h>


#define RANDOM_STREAM_PIPE "/usr/local/src/random_server/pspot_random_stream.pipe" 

CRandomGenerator* CRandomGenerator::pInst_ = NULL;

CRandomGenerator::CRandomGenerator()
  :
  random_fd_(-1)
{
  pInst_ = this;
}

CRandomGenerator::~CRandomGenerator()
{
  pInst_ = NULL;
}

// Return single random number
int CRandomGenerator::getRand()
{
#ifndef _WIN32
  if (random_fd_ > -1)
  {
    // Read a number from the random stream
    int ch = 0;
    int rc = read(random_fd_, &ch, sizeof(int));
    if (rc == sizeof(int))
    {
      return ch;
    }
    else if (rc == -1)
    {
      perror("read");
      // Error occurred; this should never happen
      // XXX TODO: reinitialize stream
      close(random_fd_);
      random_fd_ = -1;
    }
    else
    {
      // Random stream was empty - will retry on
      // next call
    }
  }
#endif

  // Random stream not available, resort to
  // standard rand() - should never happen!
  int ch = rand();
  return ch;
}


// Seed the random number generator
void CRandomGenerator::seedRand(unsigned int s)
{
  // Initialize standard software randomizer,
  // just in case
  srand(s);

#ifndef _WIN32
  // Create the socket to the random stream
  random_fd_ = open(RANDOM_STREAM_PIPE,
		    O_RDONLY);

  if (random_fd_ == -1)
  {
    // Random stream not available, resort to 
    // standard rand() - should never happen!
    Sys_LogError("Can't open random generator file ");
    return; 
  }
#endif
}



