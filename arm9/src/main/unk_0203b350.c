// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0203b350.h"

extern u32 func_0203b240(u32 param1);
extern void func_0203b410(u32 param1);
extern s32 func_0203b72c(void);
extern void * func_0203b7f4(void);
extern void func_0203b7cc(void);
extern void * func_0203b83c(u32 param1);


void * func_0203b350(u32 param1) {
   if (func_0203b83c(param1) == NULL) {
      return NULL;
   }

   void * ret = func_0203b7f4();
   if (ret != NULL) {
      return ret;
   }

   if ((param1 & 1) == 0) {
      return NULL;
   }

   if (func_0203b72c() > 0) {
      while (func_0203b240(0) != 0) {}

      if (func_0203b7f4() != NULL)
         return;
   } else {
      func_0203b410(1);
   }

   func_0203b7cc();

   do {
      func_0203b240(1);
      ret = func_0203b7f4();
   } while (ret == NULL);

   return ret;
}
