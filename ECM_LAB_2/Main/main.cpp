#include <stdlib.h>
#include "CalcLn.h"
#include <stdio.h>
#include <processthreadsapi.h>
#include <cassert>

int main(int argc, char* argv[]) {
  double arg = 0.8;
  double membersAmount = 100;
  assert(argc == 2);

  int iterationAmount = atoi(argv[1]);
  _FILETIME lpCreationTimeStart;
  _FILETIME lpExitTimeStart;
  _FILETIME lpKernelTimeStart;
  _FILETIME lpUserTimeStart;

  HANDLE Process = GetCurrentProcess();
  GetProcessTimes(Process, &lpCreationTimeStart, &lpExitTimeStart, &lpKernelTimeStart, &lpUserTimeStart);

  for (size_t i = 0; i < iterationAmount; i++) {
    CalcLn(arg, membersAmount);
  }

  _FILETIME lpCreationTimeFinish;
  _FILETIME lpExitTimeFinish;
  _FILETIME lpKernelTimeFinish;
  _FILETIME lpUserTimeFinish;
  GetProcessTimes(Process, &lpCreationTimeFinish, &lpExitTimeFinish, &lpKernelTimeFinish, &lpUserTimeFinish);

  printf("%lf", double(lpUserTimeFinish.dwLowDateTime - lpUserTimeStart.dwLowDateTime) / 10'000'000);
  return EXIT_SUCCESS;
}
