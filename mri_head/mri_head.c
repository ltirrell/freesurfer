/**
 * @file  mri_head.c
 * @brief REPLACE_WITH_ONE_LINE_SHORT_DESCRIPTION
 *
 * REPLACE_WITH_LONG_DESCRIPTION_OR_REFERENCE
 */
/*
 * Original Author: REPLACE_WITH_FULL_NAME_OF_CREATING_AUTHOR 
 * CVS Revision Info:
 *    $Author: nicks $
 *    $Date: 2006/12/29 02:09:06 $
 *    $Revision: 1.6 $
 *
 * Copyright (C) 2002-2007,
 * The General Hospital Corporation (Boston, MA). 
 * All rights reserved.
 *
 * Distribution, usage and copying of this software is covered under the
 * terms found in the License Agreement file named 'COPYING' found in the
 * FreeSurfer source code root directory, and duplicated here:
 * https://surfer.nmr.mgh.harvard.edu/fswiki/FreeSurferOpenSourceLicense
 *
 * General inquiries: freesurfer@nmr.mgh.harvard.edu
 * Bug reports: analysis-bugs@nmr.mgh.harvard.edu
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#include "mri.h"
#include "macros.h"
#include "error.h"
#include "diag.h"
#include "proto.h"
#include "fio.h"
#include "mri_conform.h"
#include "mri_identify.h"
#include "version.h"

#define ACTION_NONE  0
#define ACTION_USAGE  1
#define ACTION_IDENTIFY  2
#define ACTION_READ  3

void get_options(int argc, char *argv[]);
void usage(int exit_val);
void dummy_identify(void);
void dummy_read(void);

char *Progname;

static char *prog_name, *short_prog_name;
static char *fname = NULL;

static int action = ACTION_NONE;

int main(int argc, char *argv[]) {
  int nargs;

  /* rkt: check for and handle version tag */
  nargs = handle_version_option (argc, argv, "$Id: mri_head.c,v 1.6 2006/12/29 02:09:06 nicks Exp $", "$Name:  $");
  if (nargs && argc - nargs == 1)
    exit (0);
  argc -= nargs;

  prog_name = argv[0];
  short_prog_name = strrchr(prog_name, '/');
  short_prog_name = (short_prog_name == NULL ? prog_name : short_prog_name + 1);
  Progname = short_prog_name;

  get_options(argc, argv);

  if (action == ACTION_USAGE)
    usage(0);
  else if (action == ACTION_IDENTIFY)
    dummy_identify();
  else if (action == ACTION_READ)
    dummy_read();
  else
    usage(1);

  exit(0);

}  /*  end main()  */

void get_options(int argc, char *argv[]) {

  int i;

  for (i = 1;i < argc;i++) {
    if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "-?") == 0 || strcmp(argv[i], "-h") == 0)
      action = ACTION_USAGE;
    else if (strcmp(argv[i], "-identify") == 0)
      action = ACTION_IDENTIFY;
    else if (strcmp(argv[i], "-read") == 0)
      action = ACTION_READ;
    else
      fname = argv[i];
  }

}  /*  end get_options()  */

void usage(int exit_val) {

  FILE *fout;

  fout = (exit_val ? stderr : stdout);

  fprintf(fout, "usage: %s -identify filename\n", short_prog_name);
  fprintf(fout, "       %s -read filename\n", short_prog_name);
  fprintf(fout, "       %s -h|-u|-?\n", short_prog_name);

  exit(exit_val);

}  /*  end usage()  */

void dummy_identify(void) {

  int type;
  struct stat stat_buf;

  if (fname == NULL)
    usage(1);

  if (stat(fname, &stat_buf) < 0) {
    printf("fail\n");
    printf("can't stat file %s\n",fname);
    exit(1);
  }

  type = mri_identify(fname);

  if (type < 0) {
    printf("fail\nunknown file type\n");
    exit(1);
  } else if (type == MRI_CORONAL_SLICE_DIRECTORY)
    printf("succeed\ncoronal slice directory\n");
  else if (type == GENESIS_FILE)
    printf("succeed\ngenesis\n");
  else if (type == GE_LX_FILE)
    printf("succeed\nGE LX\n");
  else if (type == MRI_MGH_FILE)
    printf("succeed\nmgh\n");
  else if (type == MRI_MINC_FILE)
    printf("succeed\nminc\n");
  else if (type == MRI_ANALYZE_FILE)
    printf("succeed\nanalyze\n");
  else if (type == SIEMENS_FILE)
    printf("succeed\nsiemens\n");
  else if (type == BRIK_FILE)
    printf("succeed\nbrik\n");
  else if (type == BSHORT_FILE)
    printf("succeed\nbshort\n");
  else if (type == SDT_FILE)
    printf("succeed\nsdt\n");
  else {
    printf("fail\n");
    printf("%s: positive file type, but unknown to this program\n", short_prog_name);
    printf("%s: yell at your friendly neighborhood programmer\n", short_prog_name);
    exit(1);
  }

}  /*  end dummy_identify()  */

void dummy_read(void) {

  MRI *mri;

  if (fname == NULL)
    usage(1);

  freopen("/dev/null", "w", stderr);

  mri = MRIreadInfo(fname);

  if (mri == NULL) {
    printf("fail\n");
    printf("couldn't open or determine file type of %s\n", fname);
    exit(1);
  }

  printf("succeed\n");
  MRIdump(mri, stdout);

  MRIfree(&mri);

}  /*  end dummy_read()  */

/*  EOF  */
