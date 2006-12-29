/**
 * @file  ScubaToolState.h
 * @brief REPLACE_WITH_ONE_LINE_SHORT_DESCRIPTION
 *
 * REPLACE_WITH_LONG_DESCRIPTION_OR_REFERENCE
 */
/*
 * Original Author: REPLACE_WITH_FULL_NAME_OF_CREATING_AUTHOR 
 * CVS Revision Info:
 *    $Author: nicks $
 *    $Date: 2006/12/29 02:09:14 $
 *    $Revision: 1.23 $
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


#ifndef ScubaToolState_h
#define ScubaToolState_h

#include "IDTracker.h"
#include "TclCommandManager.h"

class ScubaToolState : public TclCommandListener, public IDTracker<ScubaToolState> {

public:
  ScubaToolState();
  virtual ~ScubaToolState();

  enum Mode { navigation, plane, marker, voxelEditing, voxelFilling,
              roiEditing, roiFilling, straightPath, edgePath };
  enum Shape { voxel, square, circle };
  enum FuzzinessType { seed, gradient };

  // Accessors.
  void SetMode ( Mode iMode ) {
    mMode = iMode;
  }
  Mode GetMode () {
    return mMode;
  }

  // Target.
  void SetTargetLayer ( int iTarget ) {
    mTargetLayer = iTarget;
  }
  int GetTargetLayer () {
    return mTargetLayer;
  }

  // Editing values.
  void SetNewValue ( float iValue ) {
    mNewValue = iValue;
  }
  float GetNewValue () {
    return mNewValue;
  }

  void SetNewValueMinThreshold ( float i ) {
    mNewValueMinThreshold = i;
  }
  float GetNewValueMinThreshold () {
    return mNewValueMinThreshold;
  }

  void SetNewValueMaxThreshold ( float i ) {
    mNewValueMaxThreshold = i;
  }
  float GetNewValueMaxThreshold () {
    return mNewValueMaxThreshold;
  }

  void SetEraseValueMinThreshold ( float i ) {
    mEraseValueMinThreshold = i;
  }
  float GetEraseValueMinThreshold () {
    return mEraseValueMinThreshold;
  }

  void SetEraseValueMaxThreshold ( float i ) {
    mEraseValueMaxThreshold = i;
  }
  float GetEraseValueMaxThreshold () {
    return mEraseValueMaxThreshold;
  }

  void SetUseEditThreshold ( bool ib ) {
    mbUseEditThreshold = ib;
  }
  bool GetUseEditThreshold () {
    return mbUseEditThreshold;
  }

  void SetEraseValue ( float iValue ) {
    mEraseValue = iValue;
  }
  float GetEraseValue () {
    return mEraseValue;
  }

  // Brush settings.
  void SetOnlyBrushZero ( bool ibOnlyZero ) {
    mbOnlyFillZero = ibOnlyZero;
  }
  bool GetOnlyBrushZero () {
    return mbOnlyFillZero;
  }

  void SetBrushRadius ( float iRadius ) {
    mBrushRadius = iRadius;
  }
  float GetBrushRadius () {
    return mBrushRadius;
  }

  void SetBrushShape ( Shape iShape ) {
    mBrushShape = iShape;
  }
  Shape GetBrushShape () {
    return mBrushShape;
  }
  std::string GetBrushShapeAsString ();

  void SetBrush3D ( bool ib3D ) {
    mbBrush3D = ib3D;
  }
  bool GetBrush3D () {
    return mbBrush3D;
  }

  // Fill settings
  void SetFloodStopAtPaths ( bool ibStop ) {
    mbFloodStopAtPaths = ibStop;
  }
  bool GetFloodStopAtPaths () {
    return mbFloodStopAtPaths;
  }

  void SetFloodStopAtROIs ( bool ibStop ) {
    mbFloodStopAtROIs = ibStop;
  }
  bool GetFloodStopAtROIs () {
    return mbFloodStopAtROIs;
  }

  void SetFloodFuzziness ( float iFuzziness ) {
    mFloodFuzziness = iFuzziness;
  }
  float GetFloodFuzziness () {
    return mFloodFuzziness;
  }

  void SetFloodMaxDistance ( float iDistance ) {
    mFloodMaxDistance =iDistance;
  }
  float GetFloodMaxDistance () {
    return mFloodMaxDistance;
  }

  void SetFlood3D ( bool ib3D ) {
    mbFlood3D = ib3D;
  }
  bool GetFlood3D () {
    return mbFlood3D;
  }

  void SetFloodSourceCollection ( int iCol ) {
    mFloodSourceCollection = iCol;
  }
  int GetFloodSourceCollection () {
    return mFloodSourceCollection;
  }

  void SetOnlyFloodZero ( bool ibOnlyZero ) {
    mbOnlyFloodZero = ibOnlyZero;
  }
  bool GetOnlyFloodZero () {
    return mbOnlyFloodZero;
  }

  void SetFuzzinessType ( FuzzinessType iType ) {
    mFloodFuzzinessType =iType;
  }
  FuzzinessType GetFuzzinessType () {
    return mFloodFuzzinessType;
  }

  // edgePath.
  void SetEdgePathStraightBias ( float iBias ) {
    mEdgePathStraightBias = iBias;
  }
  float GetEdgePathStraightBias () {
    return mEdgePathStraightBias;
  }

  void SetEdgePathEdgeBias ( float iBias ) {
    mEdgePathEdgeBias = iBias;
  }
  float GetEdgePathEdgeBias () {
    return mEdgePathEdgeBias;
  }

  virtual TclCommandResult
  DoListenToTclCommand ( char* isCommand, int iArgc, char** iasArgv );

protected:

  Mode mMode;

  // Layer target.
  int mTargetLayer;

  // Brush settings.
  float mBrushRadius;
  Shape mBrushShape;
  bool mbBrush3D;

  // Voxel edit settings.
  float mNewValue;
  float mNewValueMinThreshold;
  float mNewValueMaxThreshold;
  float mEraseValue;
  float mEraseValueMinThreshold;
  float mEraseValueMaxThreshold;
  bool mbUseEditThreshold;
  float mbOnlyFillZero;

  // Flood settings.
  bool mbFloodStopAtPaths;
  bool mbFloodStopAtROIs;
  float mFloodFuzziness;
  FuzzinessType mFloodFuzzinessType;
  float mFloodMaxDistance;
  bool mbFlood3D;
  int mFloodSourceCollection;
  bool mbOnlyFloodZero;

  // Edge path settings.
  float mEdgePathStraightBias;
  float mEdgePathEdgeBias;
};


#endif
