#include "cmds.h"
#include <stdlib.h>

static int sup_cmd_info_handler(int argc, char **argv);
static int sup_cmd_time_handler(int argc, char **argv);

sup_cmd_t sup_cmds[] = {
  { "info", "file1 file2 file3 ...",
    sup_cmd_info_handler, 1, 0},

  { "time", "infile outfile basepts newpts",
    sup_cmd_time_handler, 4, 4},

  { NULL, NULL,
    NULL, 0, 0}
};

static int
sup_cmd_info_handler(int argc, char **argv)
{
  int       i;

  for (i = 0; i < argc; i++) {
    printf("#%s\n", argv[i]);
    sup_run(argv[i], sup_info, NULL);
  }

  return SUP_OK;
}

static int
sup_cmd_time_handler(int argc, char **argv)
{
  sup_time_arg_t    arg;

  arg.fout = fopen(argv[1], "w");
  if (arg.fout == NULL) {
    perror("error open output file");
    return 2;
  }

  arg.orig = atoi(argv[2]);
  arg.newv = atoi(argv[3]);
  arg.diff = arg.newv - arg.orig;

  printf("adjust from %u to %u, diff %d\n", arg.orig, arg.newv, arg.diff);

  sup_run(argv[0], sup_time_fix, &arg);

  fclose(arg.fout);

  return SUP_OK;
}
