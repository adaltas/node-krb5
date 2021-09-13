#ifndef KRB5_ZOS_H_
#define KRB5_ZOS_H_
#include <_Nascii.h>

class __ae_runmode {
  int mode;

public:
  __ae_runmode(int new_mode) { mode = __ae_thread_swapmode(new_mode); }
  ~__ae_runmode() { __ae_thread_swapmode(mode); }
};
#endif
