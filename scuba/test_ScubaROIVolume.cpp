/**
 * @file  test_ScubaROIVolume.cpp
 * @brief REPLACE_WITH_ONE_LINE_SHORT_DESCRIPTION
 *
 * REPLACE_WITH_LONG_DESCRIPTION_OR_REFERENCE
 */
/*
 * Original Author: REPLACE_WITH_FULL_NAME_OF_CREATING_AUTHOR 
 * CVS Revision Info:
 *    $Author: nicks $
 *    $Date: 2006/12/29 02:09:15 $
 *    $Revision: 1.5 $
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


#include "ScubaROIVolume.h"
#include "Scuba-impl.h"

char* Progname = "test_ScubaROIVolume";

using namespace std;

#define Assert(x,s)   \
  if(!(x)) { \
  stringstream ss; \
  ss << "Line " << __LINE__ << ": " << s; \
  cerr << ss.str().c_str() << endl; \
  throw runtime_error( ss.str() ); \
  }


class ScubaROIVolumeTester {
public:
  void Test( Tcl_Interp* iInterp );
};

void
ScubaROIVolumeTester::Test ( Tcl_Interp* iInterp ) {

  stringstream ssError;

  try {

    ScubaROIVolume roi;

    int bounds[3];
    try {
      bounds[0] = -1;
      bounds[1] = 1;
      bounds[2] = 2;
      roi.SetROIBounds( bounds );
      throw runtime_error( "Illegal SetROIBounds didn't throw" );
    } catch (...) {}

    bounds[0] = 10;
    bounds[1] = 11;
    bounds[2] = 12;
    roi.SetROIBounds( bounds );
    Assert( (10 == roi.mBounds[0] && 11 == roi.mBounds[1] &&
             12 == roi.mBounds[2]), "SetROIBounds didn't work" );
    Assert( (NULL != roi.mVoxels), "SetROIBounds didn't create mVoxels" );

    Assert( (0 == roi.NumSelectedVoxels()),
            "NumSelectedVoxels didn't return 0" );

    int cSelections = 0;
    int voxel[3];
    for ( int nZ = 0; nZ < bounds[2]; nZ++ ) {
      for ( int nY = 0; nY < bounds[1]; nY++ ) {
        for ( int nX = 0; nX < bounds[0]; nX++ ) {
          voxel[0] = nX;
          voxel[1] = nY;
          voxel[2] = nZ;
          if ( nZ % 2 && nY % 2 && nX % 2 ) {
            roi.SelectVoxel( voxel );
            cSelections++;
          } else {
            roi.UnselectVoxel( voxel );
          }
        }
      }
    }

    Assert( (cSelections == roi.NumSelectedVoxels()),
            "NumSelectedVoxels didn't return proper number" );

    for ( int nZ = 0; nZ < bounds[2]; nZ++ ) {
      for ( int nY = 0; nY < bounds[1]; nY++ ) {
        for ( int nX = 0; nX < bounds[0]; nX++ ) {
          voxel[0] = nX;
          voxel[1] = nY;
          voxel[2] = nZ;
          if ( nZ % 2 && nY % 2 && nX % 2 ) {
            Assert( (roi.IsVoxelSelected( voxel )), "voxel not selected" );
          } else {
            Assert( (!roi.IsVoxelSelected( voxel )), "voxel selected" );
          }
        }
      }
    }

    // Check the list.
    list<Point3<int> > lSelected;
    lSelected = roi.GetSelectedVoxelList();
    list<Point3<int> >::iterator tSelected;
    for ( tSelected = lSelected.begin();
          tSelected != lSelected.end(); ++tSelected ) {
      Point3<int> voxel = *tSelected;
      if ( !(voxel[0] % 2 && voxel[1] % 2 && voxel[2] % 2) ) {
        stringstream ssErr;
        ssErr << "Voxel " << voxel << " was in selected list";
        Assert( 0, ssErr.str() );
      }
    }

    // Unselect a voxel, then check the selected list again.
    voxel[0] = 3; voxel[1] = 3; voxel[2] = 3;
    roi.UnselectVoxel( voxel );
    lSelected = roi.GetSelectedVoxelList();
    for ( tSelected = lSelected.begin();
          tSelected != lSelected.end(); ++tSelected ) {
      Point3<int> voxel = *tSelected;
      if ( voxel[0] == 3 && voxel[1] == 3 && voxel[2] == 3 ) {
        stringstream ssErr;
        ssErr << "Voxel " << voxel << " was in selected list";
        Assert( 0, ssErr.str() );
      }
    }

    try {
      voxel[0] = -1;
      roi.SelectVoxel( voxel );
      throw runtime_error( "SelectVoxel with x=-1 didn't throw" );
    } catch (...) {}

    try {
      voxel[0] = bounds[0];
      roi.SelectVoxel( voxel );
      throw runtime_error( "SelectVoxel with x=bounds[0] didn't throw" );
    } catch (...) {}

    // Reset bounds, select one voxel, and make sure it's in the list.
    bounds[0] = bounds[1] = bounds[2] = 2;
    roi.SetROIBounds( bounds );
    voxel[0] = 1; voxel[1] = 1; voxel[2] = 1;
    roi.SelectVoxel( voxel );
    lSelected = roi.GetSelectedVoxelList();
    if ( lSelected.size() != 1 ) {
      stringstream ssErr;
      ssErr << "Selected voxel list size was " << lSelected.size();
      Assert( 0, ssErr.str() );
    }
    for ( tSelected = lSelected.begin();
          tSelected != lSelected.end(); ++tSelected ) {
      Point3<int> voxel = *tSelected;
      if ( voxel[0] != 1 || voxel[1] != 1 || voxel[2] != 1 ) {
        stringstream ssErr;
        ssErr << "Voxel " << voxel << " was in selected list";
        Assert( 0, ssErr.str() );
      }
    }

  } catch ( runtime_error& e ) {
    cerr << "failed with exception: " << e.what() << endl;
    exit( 1 );
  } catch (...) {
    cerr << "failed" << endl;
    exit( 1 );
  }
}


int main ( int argc, char** argv ) {

  cerr << "Beginning test" << endl;

  try {

    Tcl_Interp* interp = Tcl_CreateInterp();
    Assert( interp, "Tcl_CreateInterp returned null" );

    int rTcl = Tcl_Init( interp );
    Assert( TCL_OK == rTcl, "Tcl_Init returned not TCL_OK" );

    TclCommandManager& commandMgr = TclCommandManager::GetManager();
    commandMgr.SetOutputStreamToCerr();
    commandMgr.Start( interp );

    ScubaROIVolumeTester tester0;
    tester0.Test( interp );
  } catch ( runtime_error& e ) {
    cerr << "failed with exception: " << e.what() << endl;
    exit( 1 );
  } catch (...) {
    cerr << "failed" << endl;
    exit( 1 );
  }

  cerr << "Success" << endl;

  exit( 0 );
}

