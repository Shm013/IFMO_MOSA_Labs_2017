#include "precomp.h"

void NtProcessStartup(void* StartupArgument) 
{ 
  UNICODE_STRING str; 
  PPEB pPeb = (PPEB)StartupArgument; 
  RtlNormalizeProcessParams(pPeb->ProcessParameters); 
  
  RtlInitUnicodeString(&str, L"Hello, world!\nCommand line is: ");
  NtDisplayString(&str);
  RtlInitUnicodeString(&str, pPeb->ProcessParameters->CommandLine.Buffer);
  NtDisplayString(&str);
  
  NtTerminateProcess(NtCurrentProcess(), 0);
}