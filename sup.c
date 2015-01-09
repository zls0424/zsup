#include "sup.h"

#include <stddef.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int sup_read_packet(void *p, int len, sup_packet_t *pkg) {
  size_t hsize = offsetof(sup_packet_t, data);
  if (len < hsize) {
    return SUP_ERROR;
  }

  if (memcmp(p, "PG", 2) != 0) {
    return SUP_ERROR;
  }

  memcpy(pkg, p, hsize);

  pkg->pts = ntohl(pkg->pts);
  pkg->dts = ntohl(pkg->dts);
  pkg->size = ntohs(pkg->size);
  pkg->data = p + hsize;

  return SUP_OK;
}

int
sup_walk(void *p, int len, sup_packet_handler_pt func, void *arg)
{
  int           iv;
  sup_packet_t  pkg;

  while (len > 0) {
    if (sup_read_packet(p, len, &pkg) != SUP_OK) {
      return SUP_ERROR;
    }

    iv = func(&pkg, p, arg);
    if (iv != SUP_OK) {
      return iv;
    }

    iv = offsetof(sup_packet_t, data) + pkg.size;
    p += iv;
    len -= iv;
  }

  return SUP_OK;
}

int
sup_run(const char *fpath, sup_packet_handler_pt func, void *arg)
{
  int           iv;
  sup_str_t     map;

  if (sup_load(fpath, &map) != SUP_OK) {
    return SUP_ERROR;
  }

  iv = sup_walk(map.data, map.len, func, arg);

  sup_close(&map);

  return iv;
}

int
sup_load(const char *file, sup_str_t *s) {
  int         fd;
  void       *p;
  struct stat st;

  if (stat(file, &st) != 0) {
    perror("error stat sup file");
    return SUP_ERROR;
  }

  fd = open(file, O_RDONLY);
  if (fd == -1) {
    perror("error open sup file");
    return SUP_ERROR;
  }

  p = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);

  close(fd);

  if (p == NULL) {
    perror("error mmap sup file");
    return SUP_ERROR;
  }

  s->data = p;
  s->len = st.st_size;

  return SUP_OK;
}

void
sup_close(sup_str_t *s)
{
  if (s->data) {
    munmap(s->data, s->len);
    s->data = NULL;
  }
}

int
sup_parse_pcs(sup_packet_t *pkg, sup_pcs_t *pcs)
{
  if (pkg->size < 11) {
    return SUP_ERROR;
  }

  memcpy(pcs, pkg->data, sizeof(sup_pcs_t));
  pcs->video_width = ntohs(pcs->video_width);
  pcs->video_height = ntohs(pcs->video_height);
  pcs->comp_id = ntohs(pcs->comp_id);

  if (pkg->size != pcs->num_of_objects * 8 + sizeof(sup_pcs_t)) {
    fprintf(stderr, "Invalid PCS segment length\n");
    return SUP_ERROR;
  }

  return SUP_OK;
}

char *
sup_pts2time(uint32_t pts)
{
  int           iv[4], ms;
  static char   buf[sizeof("XX00:00:00.000")];

  ms = sup_pts2ms(pts);

  iv[0] = ms / 0x36ee80;
  ms -= iv[0] * 60 * 60 * 1000;
  iv[1] = ms / 60000;
  ms -= iv[1] * 60 * 1000;
  iv[2] = ms / 1000;
  ms -= iv[2] * 1000;
  iv[3] = ms;

  snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%03d", iv[0], iv[1], iv[2], iv[3]);
  return buf;
}

char *
sup_state2name(int s)
{
  switch (s) {
    case SUP_PCS_STATE_EPOCH_START:
      return "START";

    case SUP_PCS_STATE_NORMAL:
      return "NORMAL";

    case SUP_PCS_STATE_EPOCH_CONTINUE:
      return "CONTINUE";

    case SUP_PCS_STATE_ACQU_POINT:
      return "ACQU";

    default:
      break;
  }

  return "UNK";
}
