#include <_Nascii.h>

typedef u_int uint;

class __ae_runmode {
  int mode;

public:
  __ae_runmode(int new_mode) { mode = __ae_thread_swapmode(new_mode); }
  ~__ae_runmode() { __ae_thread_swapmode(mode); }
};
