#include "cmds.h"
#include <stdlib.h>

static int sup_cmd_info_handler(int argc, char **argv);
static int sup_cmd_time_handler(int argc, char **argv);

sup_cmd_t sup_cmds[] = {
  { "info", "file1 file2 file3 ...",
    sup_cmd_info_handler, 1, 0},

  { "time", "infile outfile base_pts delta(ms)",
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
  int               delta_ms;
  sup_time_arg_t    arg;

  arg.fout = fopen(argv[1], "w");
  if (arg.fout == NULL) {
    perror("error open output file");
    return 2;
  }

  arg.base = atoi(argv[2]);
  delta_ms = atoi(argv[3]);

  arg.diff = sup_ms2pts(delta_ms);

  printf("adjust from %u to %u, with %dms\n",
         arg.base, arg.base + arg.diff, delta_ms);

  sup_run(argv[0], sup_time_fix, &arg);

  fclose(arg.fout);

  return SUP_OK;
}
