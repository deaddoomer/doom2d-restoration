#include "dll.h"

DLLEXPORT char *av_ez_msg[]={
  "Error",
  "Internal error",
  "VGA error",
  "Error loading %s from %s",
  "Error loading animation %s",
  "Error loading sound %s"
}, *av_err_msg[]={
  "Error 0",
  "Bad data format",
  "Resource not found",
  "Hardware error"
};
