/* $Id: computer.h,v 1.6 2001/06/05 12:19:45 jorge Exp $ */

#ifndef _COMPUTER_H_
#define _COMPUTER_H_

#include <netinet/in.h>
#include <time.h>

#include "computer_info.h"
#include "computer_status.h"

struct computer {
  struct computer_hwinfo hwinfo;
  struct computer_status status;
  time_t lastconn;		/* Time of last connection to the master */
  unsigned char used;		/* If the record is being used or not */
};

struct database;

int computer_index_addr (void *pwdb,struct in_addr addr); /* I use pointers to void instead to struct database */
int computer_index_name (void *pwdb,char *name);          /* because if I did I would have to create a dependency loop */
int computer_index_free (void *pwdb);
int computer_available (struct computer *computer);
void computer_update_assigned (struct database *wdb,int ijob,int iframe,int icomp,int itask);
void computer_init (struct computer *computer);



#endif /* _COMPUTER_H_ */
