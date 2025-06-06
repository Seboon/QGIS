/***************************************************************************
  qgs3daxis.h
  --------------------------------------
  Date                 : March 2022
  Copyright            : (C) 2022 by Jean Felder
  Email                : jean dot felder at oslandia dot com
  ***************************************************************************
  *                                                                         *
  *   This program is free software; you can redistribute it and/or modify  *
  *   it under the terms of the GNU General Public License as published by  *
  *   the Free Software Foundation; either version 2 of the License, or     *
  *   (at your option) any later version.                                   *
  *                                                                         *
  ***************************************************************************/

#ifndef QGS3DAXIS_H
#define QGS3DAXIS_H

#include "qgis_3d.h"
#include "qgs3dmapcanvas.h"

#include "qgscoordinatereferencesystem.h"
#include <Qt3DCore/QEntity>
#include <Qt3DExtras/QText2DEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QScreenRayCaster>
#include <QVector3D>

#include <QtWidgets/QMenu>
#include "qgs3daxissettings.h"

#define SIP_NO_FILE

class QgsCameraController;
class Qgs3DMapScene;
class Qgs3DAxisRenderView;

/**
 * \ingroup qgis_3d
 * \brief Display 3D ortho axis in the main 3D view.
 *
 * Axis are displayed in a dedicated viewport which can be placed all around the main viewport.
 * Axis labels are displayed in a dedicated viewport with a specific camera to act as a billboarding layer.
 * Axis labels can be changed with the mode combo box from the navigation widget.

 * \note Not available in Python bindings
 *
 * \since QGIS 3.26
 */
class _3D_EXPORT Qgs3DAxis : public QObject
{
    Q_OBJECT
  public:
    /**
     * Default Qgs3DAxis constructor.
     *
     * \param canvas parent Qgs3DMapCanvas
     * \param parent3DScene root entity to set as parent
     * \param mapScene 3d map scene to retrieve terrain and 3d engine data
     * \param camera camera controller used to track camera movements
     * \param map 3D map settings
     */
    Qgs3DAxis( Qgs3DMapCanvas *canvas, Qt3DCore::QEntity *parent3DScene, Qgs3DMapScene *mapScene, QgsCameraController *camera, Qgs3DMapSettings *map );
    ~Qgs3DAxis() override;

    /**
     * Project a 3D position from sourceCamera to a 2D position for \a destCamera.
     *
     * \a destCamera acts as a billboarding layer. The labels displayed by this process will always face the camera.
     *
     * \param sourcePos 3D label coordinates
     * \param sourceCamera main view camera
     * \param destCamera billboarding camera
     */
    QVector3D from3DTo2DLabelPosition( const QVector3D &sourcePos, Qt3DRender::QCamera *sourceCamera, Qt3DRender::QCamera *destCamera );

    /**
     * Used as callback from renderview when viewport scale factor changes
     * \since QGIS 3.44
     */
    void onViewportScaleFactorChanged( double scaleFactor );

    /**
     * Returns if the 3D axis controller will handle the specified \a event.
     *
     * - TRUE when event is key within Ctrl+[2345689] to handle view orientation
     * - FALSE when event is mouse click within the 3Daxis space to handle menu and orientation by clicking on cube faces
     */
    bool handleEvent( QEvent *event );

  public slots:

    //! Force update of the axis and the viewport when a setting has changed
    void onAxisSettingsChanged();

  private slots:

    void onCameraUpdate();
    void onAxisViewportSizeUpdate();

    // axis picking and menu
    void onTouchedByRay( const Qt3DRender::QAbstractRayCaster::Hits &hits );
    void onAxisModeChanged( Qgs3DAxisSettings::Mode mode );

  private:
    void createAxisScene();
    void createAxis( Qt::Axis axis );
    void createCube();
    void setEnableCube( bool show );
    void setEnableAxis( bool show );
    void updateAxisLabelPosition();
    void updateAxisLabelText( Qt3DExtras::QText2DEntity *textEntity, const QString &text );
    QFont createFont( int pointSize );

    void constructAxisScene( Qt3DCore::QEntity *parent3DScene );
    void constructLabelsScene( Qt3DCore::QEntity *parent3DScene );

    Qt3DExtras::QText2DEntity *addCubeText( const QString &text, float textHeight, float textWidth, const QFont &font, const QMatrix4x4 &rotation, const QVector3D &translation );

    // axis picking and menu
    void init3DObjectPicking();
    void createMenu();
    void hideMenu();
    void displayMenuAt( const QPoint &position );

    Qgs3DMapSettings *mMapSettings = nullptr;
    Qgs3DMapCanvas *mCanvas = nullptr;
    Qgs3DMapScene *mMapScene = nullptr;
    QgsCameraController *mCameraController = nullptr;

    float mCylinderLength = 40.0f;
    int mFontSize = 12;

    Qgs3DAxisRenderView *mRenderView = nullptr;
    Qt3DCore::QEntity *mAxisSceneEntity = nullptr;
    Qt3DRender::QCamera *mAxisCamera = nullptr;

    Qt3DCore::QEntity *mAxisRoot = nullptr;
    Qt3DCore::QEntity *mCubeRoot = nullptr;
    QList<Qt3DExtras::QText2DEntity *> mCubeLabels;

    Qt3DExtras::QText2DEntity *mTextX = nullptr;
    Qt3DExtras::QText2DEntity *mTextY = nullptr;
    Qt3DExtras::QText2DEntity *mTextZ = nullptr;
    QVector3D mTextCoordX;
    QVector3D mTextCoordY;
    QVector3D mTextCoordZ;
    Qt3DCore::QTransform *mTextTransformX = nullptr;
    Qt3DCore::QTransform *mTextTransformY = nullptr;
    Qt3DCore::QTransform *mTextTransformZ = nullptr;
    QgsCoordinateReferenceSystem mCrs;
    QVector3D mPreviousVector;
    double mAxisScaleFactor = 1.0;

    Qt3DRender::QCamera *mTwoDLabelCamera = nullptr;
    Qt3DCore::QEntity *mTwoDLabelSceneEntity = nullptr;

    // axis picking and menu
    Qt3DRender::QScreenRayCaster *mScreenRayCaster = nullptr;
    bool mIsDragging = false;
    bool mHasClicked = false;
    QPoint mLastClickedPos;
    Qt::MouseButton mLastClickedButton;
    QCursor mPreviousCursor = Qt::ArrowCursor;
    Qt3DRender::QPickingSettings::PickMethod mDefaultPickingMethod;
    QMenu *mMenu = nullptr;
};

#endif // QGS3DAXIS_H
