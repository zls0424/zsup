#include "sup.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int
sup_time_fix(sup_packet_t *pkg, void *p, void *d)
{
  uint32_t           newv;
  sup_packet_t       dummy;
  sup_time_arg_t    *arg = d;

  if (pkg->type != SUP_SEGMENT_PCS || pkg->pts < arg->base) {
    fwrite(p, sup_packet_hdr_len + pkg->size, 1, arg->fout);
    return SUP_OK;
  }

  newv = pkg->pts + arg->diff;
  printf("Adjust [%u] -> [%u]\n", pkg->pts, newv);

  memcpy(&dummy, p, sup_packet_hdr_len);
  dummy.pts = htonl(newv);

  fwrite(&dummy, sup_packet_hdr_len, 1, arg->fout);
  fwrite(p + sup_packet_hdr_len, pkg->size, 1, arg->fout);

  return SUP_OK;
}

int
sup_info(sup_packet_t *pkg, void *p, void *arg)
{
  sup_pcs_t         pcs;

  if (pkg->type == SUP_SEGMENT_PCS) {
    if (sup_parse_pcs(pkg, &pcs) != SUP_OK) {
      return SUP_ERROR;
    }

    printf("<PCS> [%s] %d %s\n",
           sup_pts2time(pkg->pts),
           pkg->pts,
           sup_state2name(pcs.comp_state));
    return SUP_OK;
  }

  if (pkg->type == SUP_SEGMENT_PDS) {
    printf("<PDS>\n");
    return SUP_OK;
  }

  if (pkg->type == SUP_SEGMENT_WDS) {
    printf("<WDS>\n");
    return SUP_OK;
  }

  if (pkg->type == SUP_SEGMENT_ODS) {
    printf("<ODS>\n");
    return SUP_OK;
  }

  if (pkg->type == SUP_SEGMENT_END) {
    printf("<END>\n");
    return SUP_OK;
  }

  return SUP_OK;
}
