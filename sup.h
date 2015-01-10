#ifndef __SUP_H
#define __SUP_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stddef.h>

#define SUP_OK       0
#define SUP_ERROR   -1

#define SUP_SEGMENT_PCS 0x16    /* Composition info */
#define SUP_SEGMENT_PDS 0x14    /* Palette*/
#define SUP_SEGMENT_WDS 0x17    /* Windows info */
#define SUP_SEGMENT_ODS 0x15    /* Caption image */
#define SUP_SEGMENT_END 0x80    /* End of composition */
#define SUP_SEGMENT_UNKNOWN -1

#define SUP_PCS_STATE_NORMAL          0x00  /* Normal: doesn't have to be complete */
#define SUP_PCS_STATE_ACQU_POINT      0x40  /* Acquisition point */
#define SUP_PCS_STATE_EPOCH_START     0x80  /* Epoch start, clears the screen */
#define SUP_PCS_STATE_EPOCH_CONTINUE  0xc0  /* Epoch continue */
#define SUP_PCS_STATE_UNKNOWN -1

#pragma pack(1)
typedef struct {
    uint16_t  magic;          /* Packet marker: "PG" */
    uint32_t  pts;            /* PTS - presentation time stamp */
    uint32_t  dts;            /* DTS - decoding time stamp */
    uint8_t   type;           /* Segment type */
    uint16_t  size;           /* Segment length (bytes following until next PG) */
    void     *data;
} sup_packet_t;

#define sup_packet_hdr_len offsetof(sup_packet_t, data)

typedef struct {
  size_t      len;
  void       *data;
} sup_str_t;

typedef struct {
  FILE      *fout;
  uint32_t   base;
  int32_t    diff;
} sup_time_arg_t;

typedef struct {
  uint16_t  video_width;
  uint16_t  video_height;
  uint8_t   frame_rate;
  uint16_t  comp_id;
  uint8_t   comp_state;
  uint8_t   palette_flag;
  uint8_t   palette_id;
  uint8_t   num_of_objects;
} sup_pcs_t;
#pragma pack()

int sup_parse_pcs(sup_packet_t *pkg, sup_pcs_t *pcs);

typedef int (*sup_packet_handler_pt)(sup_packet_t *pkg, void *p, void *arg);

static inline int sup_pts2ms(uint32_t pts) {
  return (pts + 45) / 90;
}

char *sup_pts2time(uint32_t pts);

static inline int sup_ms2pts(uint32_t ms) {
  return (ms * 90) - 45;
}

int sup_run(const char *fpath, sup_packet_handler_pt func, void *arg);
int sup_walk(void *p, int len, sup_packet_handler_pt func, void *arg);
int sup_load(const char *file, sup_str_t *s);
void sup_close(sup_str_t *s);

int sup_time_fix(sup_packet_t *pkg, void *p, void *arg);
int sup_info(sup_packet_t *pkg, void *p, void *arg);
char * sup_state2name(int s);

#endif
