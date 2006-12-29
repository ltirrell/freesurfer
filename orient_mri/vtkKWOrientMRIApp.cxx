/**
 * @file  vtkKWOrientMRIApp.cxx
 * @brief REPLACE_WITH_ONE_LINE_SHORT_DESCRIPTION
 *
 * REPLACE_WITH_LONG_DESCRIPTION_OR_REFERENCE
 */
/*
 * Original Author: REPLACE_WITH_FULL_NAME_OF_CREATING_AUTHOR 
 * CVS Revision Info:
 *    $Author: nicks $
 *    $Date: 2006/12/29 02:09:11 $
 *    $Revision: 1.3 $
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


#include "vtkKWOrientMRIApp.h"
#include "vtkKWOrientMRIWindow.h"
#include "vtkObjectFactory.h"

using namespace std;

const char* vtkKWOrientMRIApp::sMainWindowWidthRegKey = "MainWindowWidth";
const char* vtkKWOrientMRIApp::sMainWindowHeightRegKey = "MainWindowHeight";

vtkStandardNewMacro( vtkKWOrientMRIApp );
vtkCxxRevisionMacro( vtkKWOrientMRIApp, "$Revision: 1.3 $" );

vtkKWOrientMRIApp::vtkKWOrientMRIApp () :
    vtkKWApplication(),
    mWindow( NULL ),
    mMainWindowWidth( 600 ),
    mMainWindowHeight( 600 ) {}

vtkKWOrientMRIApp::~vtkKWOrientMRIApp () {

  mWindow->Delete();
}

void
vtkKWOrientMRIApp::Start ( int argc, char* argv[] ) {

  // Create the main window.
  mWindow = vtkKWOrientMRIWindow::New();
  mWindow->SetApplication( this );
  this->AddWindow( mWindow );
  mWindow->Create();

  SetName( "orient_mri" );
  RestoreApplicationSettingsFromRegistry();

  mWindow->SetSize( mMainWindowWidth, mMainWindowHeight );

  SetHelpDialogStartingPage("https://surfer.nmr.mgh.harvard.edu/fswiki/orient_5fmri");

  mWindow->Display();

  // Command line arguments.
  for ( int i = 1; i < argc; i++ ) {

    // If this isn't a dash-option, continue.
    if ( argv[i] && *argv[i] != '-' ) {
      cerr << "Unrecognized option: " << argv[i] << endl;
    }

    // Now that we're sure it's a dash-option, remove the dashes.
    string arg( argv[i] );
    if ( arg[0] == '-' ) arg = arg.replace( 0, 1, "" );
    if ( arg[0] == '-' ) arg = arg.replace( 0, 1, "" );

    // Check the possibilites.
    if ( arg == "v" || arg == "volume") {

      // Volume. All following arguments are volume names until the
      // next dash. Keep loading volumes until we see another -. For
      // each one, try to load it. If failed, put up an error
      // message and try the next one.
      do {
        string s( argv[++i] );
        try {
          LoadVolume( s.c_str() );
        } catch ( exception& e ) {
          cerr << "Error loading volume \"" << s << "\": " << e.what();
        }
      } while ( i+1 < argc && argv[i+1][0] != '-' );

    }
  }

  this->Superclass::Start( argc, argv );
}

void
vtkKWOrientMRIApp::RestoreApplicationSettingsFromRegistry () {

  vtkKWApplication::RestoreApplicationSettingsFromRegistry();

  if ( this->HasRegistryValue(2,"MainWindowWidth",sMainWindowWidthRegKey) ) {
    mMainWindowWidth =
      this->GetIntRegistryValue(2,"MainWindowWidth",sMainWindowWidthRegKey);
  }

  if ( this->HasRegistryValue(2,"MainWindowHeight",sMainWindowHeightRegKey) ) {
    mMainWindowHeight =
      this->GetIntRegistryValue(2,"MainWindowHeight",sMainWindowHeightRegKey);
  }

  if ( NULL != mWindow ) {
    mWindow->SetSize( mMainWindowWidth, mMainWindowHeight );
  }
}

void
vtkKWOrientMRIApp::SaveApplicationSettingsToRegistry () {

  vtkKWApplication::SaveApplicationSettingsToRegistry();

  if ( NULL != mWindow ) {
    this->mWindow->GetSize( &mMainWindowWidth, &mMainWindowHeight );
  }

  this->SetRegistryValue( 2, "MainWindowWidth", sMainWindowWidthRegKey,
                          "%d", mMainWindowWidth );

  this->SetRegistryValue( 2, "MainWindowHeight", sMainWindowHeightRegKey,
                          "%d", mMainWindowHeight );
}

void
vtkKWOrientMRIApp::LoadVolume ( const char* ifnVolume ) {
  if ( mWindow )
    mWindow->LoadVolume( ifnVolume );
}
