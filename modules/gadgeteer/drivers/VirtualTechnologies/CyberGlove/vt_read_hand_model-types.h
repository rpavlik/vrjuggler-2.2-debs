#ifndef VT_READ_HAND_MODEL_TYPES_H
#define VT_READ_HAND_MODEL_TYPES_H
/*  FILE: vt_read_hand_model-types.h                                */
/*                                                                  */
/*  type declarations and constant defintions for                   */
/*  vt_read_hand_model.c                                            */
/*                                                                  */
/*  The SurfaceModel structure contains a boundary rep. and the ids */
/*  of associated GL display lists                                  */
/*                                                                  */
/*  ==============================================================  */
/*  --  Copyright (C) 1992, 1993 Virtual Technologies           --  */
/*                                                              --  */
/*  --    Author:  Larry Edwards                                --  */
/*  ==============================================================  */

#include <gadget/Devices/DriverConfig.h>
#include <drivers/VirtualTechnologies/CyberGlove/vt_read_object-types.h>

typedef struct
{
  int palm_id;
  objstruct palm;       /* model of the main body of the hand */
  int forearm_id;
  objstruct forearm;        /* model of the forearm, strangely enough... */
  int digit_id[5][3];
  objstruct digit[5][3];    /* models of all the digits */
} SurfaceModelStruct,*SurfaceModel;

#endif              /* VT_READ_HAND_MODEL_TYPES_H */
