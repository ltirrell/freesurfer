/**
 * @file  mrisp_paint.c
 * @brief REPLACE_WITH_ONE_LINE_SHORT_DESCRIPTION
 *
 * REPLACE_WITH_LONG_DESCRIPTION_OR_REFERENCE
 */
/*
 * Original Author: REPLACE_WITH_FULL_NAME_OF_CREATING_AUTHOR 
 * CVS Revision Info:
 *    $Author: nicks $
 *    $Date: 2006/12/29 02:09:11 $
 *    $Revision: 1.7 $
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
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "macros.h"
#include "error.h"
#include "diag.h"
#include "proto.h"
#include "mrisurf.h"
#include "mri.h"
#include "macros.h"
#include "version.h"

static char vcid[] = "$Id: mrisp_paint.c,v 1.7 2006/12/29 02:09:11 nicks Exp $";

int main(int argc, char *argv[]) ;

static int  get_option(int argc, char *argv[]) ;
static void usage_exit(void) ;
static void print_usage(void) ;
static void print_help(void) ;
static void print_version(void) ;

char *Progname ;

static int normalize = 0 ;
static int variance =  0 ;
static int navgs = 0 ;
static int sqrt_flag = 0 ;

static char *surface_names[] = {
                                 "inflated",
                                 "smoothwm",
                                 "smoothwm"
                               } ;

static int field_no = -1;
static char subjects_dir[STRLEN] ;
static char *hemi=NULL;
static char *subject_name;

int
main(int argc, char *argv[]) {
  char         **av, *surf_fname, *template_fname, *out_fname, *cp;
  int          n,ac, nargs, param_no = 0 ;
  float        sse,var;
  char fname[STRLEN];
  MRI_SURFACE  *mris ,*mris_var;
  MRI_SP       *mrisp ;
  VERTEX *v;

  /* rkt: check for and handle version tag */
  nargs = handle_version_option (argc, argv, "$Id: mrisp_paint.c,v 1.7 2006/12/29 02:09:11 nicks Exp $", "$Name:  $");
  if (nargs && argc - nargs == 1)
    exit (0);
  argc -= nargs;

  Progname = argv[0] ;
  ErrorInit(NULL, NULL, NULL) ;
  DiagInit(NULL, NULL, NULL) ;

  ac = argc ;
  av = argv ;
  for ( ; argc > 1 && ISOPTION(*argv[1]) ; argc--, argv++) {
    nargs = get_option(argc, argv) ;
    argc -= nargs ;
    argv += nargs ;
  }

  if (argc < 4)
    usage_exit() ;

  template_fname = argv[1] ;
  surf_fname = argv[2] ;
  out_fname = argv[3] ;

  fprintf(stderr, "reading surface from %s...\n", surf_fname) ;
  mris = MRISread(surf_fname) ;
  if (!mris)
    ErrorExit(ERROR_NOFILE, "%s: could not read surface file %s",
              Progname, surf_fname) ;

  if (normalize) {
    cp = strchr(template_fname, '#') ;
    if (cp)   /* # explicitly given */
    {
      param_no = atoi(cp+1) ;
      *cp = 0 ;
    } else
      param_no = 0 ;
  } else {
    cp = strchr(template_fname, '#') ;
    if (cp)   /* # explicitly given */
    {
      param_no = atoi(cp+1) ;
      *cp = 0 ;
    } else
      param_no = 0 ;
  }
  fprintf(stderr, "reading template parameterization from %s...\n",
          template_fname) ;
  mrisp = MRISPread(template_fname) ;
  if (!mrisp)
    ErrorExit(ERROR_NOFILE, "%s: could not open template file %s",
              Progname, template_fname) ;

  if (normalize)
    MRISnormalizeFromParameterization(mrisp, mris, param_no) ;
  else
    MRISfromParameterization(mrisp, mris, param_no) ;

  MRISaverageCurvatures(mris, navgs) ;

  if (variance) {
    /* check if SUBJECTS_DIR is set up */
    if (!strlen(subjects_dir)) /* hasn't been set on command line */
    {
      cp = getenv("SUBJECTS_DIR") ;
      if (!cp)
        ErrorExit(ERROR_BADPARM, "%s: SUBJECTS_DIR not defined in environment",
                  Progname);
      strcpy(subjects_dir, cp) ;
    }
    mris_var=MRISclone(mris);
    /* reading or generating the field */
    if (ReturnFieldName(field_no)) {  /* read in precomputed curvature file */
      sprintf(fname, "%s/%s/surf/%s.%s", subjects_dir,subject_name,hemi,ReturnFieldName(field_no)) ;
      if (MRISreadCurvatureFile(mris_var, fname) != NO_ERROR) {
        MRISfree(&mris_var);
        ErrorExit(ERROR_BADPARM,"%s: could not load file %s\n",fname);
      }
    } else {                       /* compute curvature of surface */
      sprintf(fname, "%s/%s/surf/%s.%s", subjects_dir,subject_name,hemi,surface_names[field_no]) ;
      /*    if(parms->fields[n].field==0) */
      /*     sprintf(fname, "inflated") ; */
      /*    else */
      /*     sprintf(fname, "smoothwm") ; */
      MRISsaveVertexPositions(mris_var, CANONICAL_VERTICES) ;
      if (MRISreadVertexPositions(mris_var, fname) != NO_ERROR) {
        MRISfree(&mris_var);
        ErrorExit(ERROR_BADPARM,"%s: could not load file %s\n",fname);
      }
      MRISsetNeighborhoodSize(mris_var, -1) ;  /* back to max */
      MRIScomputeMetricProperties(mris_var) ;
      MRIScomputeSecondFundamentalForm(mris_var) ;
      MRISuseMeanCurvature(mris_var) ;
      MRISaverageCurvatures(mris_var, navgs) ;
      MRISresetNeighborhoodSize(mris_var,1);/*only use nearest neighbor distances*/
      MRISrestoreVertexPositions(mris_var, CANONICAL_VERTICES) ;
    }
    MRISnormalizeField(mris_var,IsDistanceField(field_no));
    MRISnormalizeField(mris,IsDistanceField(field_no));
    /* save curv into curvbak*/
    for (n=0 ; n < mris->nvertices; n++) {
      v=&mris_var->vertices[n];
      v->curvbak=v->curv;
    }
    /* computing variance */
    MRISfromParameterization(mrisp, mris_var, param_no+1) ;
    for (sse=0.0f,n=0 ; n < mris->nvertices; n++) {
      v=&mris_var->vertices[n];
      var=MAX(0.01,v->curv);
      v->curv = SQR(v->curvbak-mris->vertices[n].curv)/var;
      sse += v->curv;
    }
    fprintf(stderr,"XXXXXXXXXXXXXXXXXXX\n\n");
    fprintf(stderr,"The SSE for this field is %f\n",sqrt(sse/(float)mris->nvertices));
    fprintf(stderr,"\nXXXXXXXXXXXXXXXXXXX\n\n");

    MRISfree(&mris_var);
  }

  fprintf(stderr, "writing curvature file to %s...\n", out_fname) ;
  if (sqrt_flag) {
    MRIScopyCurvatureToValues(mris) ;
    MRISsqrtVal(mris) ;
    MRIScopyCurvatureFromValues(mris) ;
  }

  MRISwriteCurvature(mris, out_fname) ;

  MRISPfree(&mrisp) ;
  MRISfree(&mris) ;
  exit(0) ;
  return(0) ;  /* for ansi */
}

/*----------------------------------------------------------------------
            Parameters:

           Description:
----------------------------------------------------------------------*/
static int
get_option(int argc, char *argv[]) {
  int    nargs = 0 ;
  char   *option ;

  option = argv[1] + 1 ;            /* past '-' */
  if (!stricmp(option, "-help"))
    print_help() ;
  else if (!stricmp(option, "-version"))
    print_version() ;
  else if (!stricmp(option, "SDIR")) {
    strcpy(subjects_dir, argv[2]) ;
    nargs = 1 ;
    printf("using %s as subjects directory\n", subjects_dir) ;
  } else if (!stricmp(option, "variance")) {
    variance=1;
    hemi = argv[3];
    subject_name = argv[2];
    field_no = atoi(argv[4]);
    if (field_no<0) {
      fprintf(stderr,"Incorrect Field Number\n");
      exit(-1);
    }
    fprintf(stderr,"generating variance map for with field %d (%s/surf/%s.%s) \n",field_no,subject_name,hemi,ReturnFieldName(field_no));
    nargs=3;
  } else switch (toupper(*option)) {
    case 'A':
      navgs = atoi(argv[2]) ;
      nargs = 1 ;
      fprintf(stderr, "averaging curvature patterns %d times...\n", navgs) ;
      break ;
    case 'N':
      normalize = 1 ;
      fprintf(stderr, "normalizing curvature by variance.\n") ;
      break ;
    case 'W':
      Gdiag |= DIAG_WRITE ;
      break ;
    case 'S':
      printf("taking sqrt before writing...\n") ;
      sqrt_flag = 1 ;
      break ;
    case 'V':
      Gdiag_no = atoi(argv[2]) ;
      nargs = 1 ;
      break ;
    case '?':
    case 'U':
      print_usage() ;
      exit(1) ;
      break ;
    default:
      fprintf(stderr, "unknown option %s\n", argv[1]) ;
      exit(1) ;
      break ;
    }

  return(nargs) ;
}

static void
usage_exit(void) {
  print_usage() ;
  exit(1) ;
}

static void
print_usage(void) {
  fprintf(stderr,
          "usage: %s [options] <parameterization file> <input surface> <output name>\n",
          Progname) ;
}

static void
print_help(void) {
  print_usage() ;
  fprintf(stderr,
          "\nThis program paint a parameterization onto a surface and output.\n"
          "the results as a curvature file.\n") ;
  fprintf(stderr, "\nvalid options are:\n\n") ;
  exit(1) ;
}

static void
print_version(void) {
  fprintf(stderr, "%s\n", vcid) ;
  exit(1) ;
}

