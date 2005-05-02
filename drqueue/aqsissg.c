// 
// Copyright (C) 2001,2002,2003,2004 Jorge Daza Garcia-Blanes
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
// USA
// 
//
// $Id$
//

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include "aqsissg.h"
#include "libdrqueue.h"

char *aqsissg_create (struct aqsissgi *info)
{
  /* This function creates the aqsis render script based on the information given */
  /* Returns a pointer to a string containing the path of the just created file */
  /* Returns NULL on failure and sets drerrno */
  FILE *f;
  FILE *etc_aqsis_sg; 		/* The aqsis script generator configuration file */
  int fd_etc_aqsis_sg,fd_f;
  static char filename[BUFFERLEN];
  char fn_etc_aqsis_sg[BUFFERLEN]; /* File name pointing to DRQUEUE_ETC/aqsis.sg */
  char buf[BUFFERLEN];
  int size;
  char *p;			/* Scene filename without path */

  /* Check the parameters */
  if (!strlen(info->scene)) {
    drerrno = DRE_NOTCOMPLETE;
    return NULL;
  }

  p = strrchr(info->scene,'/');
  p = ( p ) ? p+1 : info->scene;
  snprintf(filename,BUFFERLEN-1,"%s/%s.%lX",info->scriptdir,p,(unsigned long int)time(NULL));

  if ((f = fopen (filename, "a")) == NULL) {
    if (errno == ENOENT) {
      /* If its because the directory does not exist we try creating it first */
      if (mkdir (info->scriptdir,0775) == -1) {
        drerrno = DRE_COULDNOTCREATE;
        return NULL;
      } else if ((f = fopen (filename, "a")) == NULL) {
        drerrno = DRE_COULDNOTCREATE;
        return NULL;
      }
    } else {
      drerrno = DRE_COULDNOTCREATE;
      return NULL;
    }
  }

  fchmod (fileno(f),0777);

  /* So now we have the file open and so we must write to it */
  fprintf(f,"#!/bin/tcsh\n\n");
  fprintf(f,"set DRQUEUE_SCENE=%s\n",info->scene);
  fprintf(f,"set RF_OWNER=%s\n",info->file_owner);
	if (info->custom_crop) {
		fprintf (f,"set CUSTOM_CROP=yes\n");
		fprintf (f,"set CROP_XMIN=%u\n",info->xmin);
		fprintf (f,"set CROP_XMAX=%u\n",info->xmax);
		fprintf (f,"set CROP_YMIN=%u\n",info->ymin);
		fprintf (f,"set CROP_YMAX=%u\n",info->ymax);
	}
  fflush (f);

  snprintf(fn_etc_aqsis_sg,BUFFERLEN-1,"%s/aqsis.sg",getenv("DRQUEUE_ETC"));
  if ((etc_aqsis_sg = fopen (fn_etc_aqsis_sg,"r")) == NULL) {
    fprintf(f,"\necho -------------------------------------------------\n");
    fprintf(f,"echo ATTENTION ! There was a problem opening: %s\n",fn_etc_aqsis_sg);
    fprintf(f,"echo Please correct the problem\n");
    fprintf(f,"echo -------------------------------------------------\n");
    fprintf(f,"\n\n");
  } else {
    fd_etc_aqsis_sg = fileno (etc_aqsis_sg);
    fd_f = fileno (f);
    while ((size = read (fd_etc_aqsis_sg,buf,BUFFERLEN)) != 0) {
      write (fd_f,buf,size);
    }
    fclose(etc_aqsis_sg);
  }

  fclose(f);

  return filename;
}


char *aqsissg_default_script_path (void)
{
  static char buf[BUFFERLEN];
  char *p;

  if (!(p = getenv("DRQUEUE_TMP"))) {
    return ("/drqueue_tmp/not/set/");
  }

#ifdef __CYGWIN	 
  if (p[strlen(p)-1] == '\\')
		snprintf (buf,BUFFERLEN-1,"%s",p);
	else
		snprintf (buf,BUFFERLEN-1,"%s\\",p);
#else
  if (p[strlen(p)-1] == '/')
		snprintf (buf,BUFFERLEN-1,"%s",p);
	else
		snprintf (buf,BUFFERLEN-1,"%s/",p);
#endif

  return buf;
}


