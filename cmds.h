#ifndef __CMDS_H
#define __CMDS_H

#include "sup.h"

typedef int (*sup_cmd_pt)(int argc, char **argv);

typedef struct {
  const char    *name;
  const char    *docs;
  sup_cmd_pt     handler;
  uint8_t        min_arg;
  uint8_t        max_arg;
} sup_cmd_t;

extern sup_cmd_t sup_cmds[];
#endif
