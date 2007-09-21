#ifndef __psi_random_generator_h__
#define __psi_random_generator_h__


class CRandomGenerator
{
public:
  // Return single random number
  int getRand();
  // Seed the random number generator
  void seedRand(unsigned int);

  CRandomGenerator();
  ~CRandomGenerator();

  static CRandomGenerator* Inst() { return pInst_; }

private:
  static CRandomGenerator* pInst_;

  int random_fd_;
};


#endif
