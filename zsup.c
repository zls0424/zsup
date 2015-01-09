#include "sup.h"
#include "cmds.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage(const char *proc) {
  sup_cmd_t       *cmd;

  fprintf(stderr, "Usage: %s command [args]\n", proc);
  
  cmd = &sup_cmds[0];
  while (cmd->name && cmd->name[0]) {
    fprintf(stderr, "  %s %s\n", cmd->name, cmd->docs ? cmd->docs : "");
    cmd ++;
  }
}

int main(int argc, char **argv) {
  int               narg;
  sup_cmd_t        *cmd;
  const char       *proc;

  proc = argv[0];

  if (argc < 2) {
    usage(proc);
    return 0;
  }

  cmd = &sup_cmds[0];

  while (cmd->name && cmd->name[0]) {
    if (strcmp(cmd->name, argv[1]) != 0) {
      cmd++;
      continue;
    }

    narg = argc - 2;

    if ((cmd->min_arg && narg < cmd->min_arg) ||
        (cmd->max_arg && narg > cmd->max_arg))
    {
      fprintf(stderr, "Error param count %d for command %s [%d,%d]\n",
              narg, cmd->name, cmd->min_arg, cmd->max_arg);
      return SUP_ERROR;
    }

    return cmd->handler(narg, &argv[2]);
  }

  usage(proc);

  return SUP_OK;
}
