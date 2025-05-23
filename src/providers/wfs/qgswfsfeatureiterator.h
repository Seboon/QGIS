/***************************************************************************
    qgswfsfeatureiterator.h
    ---------------------
    begin                : January 2013
    copyright            : (C) 2013 by Marco Hugentobler
                           (C) 2016 by Even Rouault
    email                : marco dot hugentobler at sourcepole dot ch
                           even.rouault at spatialys.com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QGSWFSFEATUREITERATOR_H
#define QGSWFSFEATUREITERATOR_H

#include "qgsfeatureiterator.h"
#include "qgswfsrequest.h"
#include "qgsgml.h"

#include "qgsbackgroundcachedfeatureiterator.h"

#include <memory>
#include <QMutex>
#include <QWaitCondition>

class QgsWFSProvider;
class QgsWFSSharedData;
class QgsVectorDataProvider;

//! Utility class to issue a GetFeature resultType=hits request
class QgsWFSFeatureHitsAsyncRequest final : public QgsWfsRequest
{
    Q_OBJECT
  public:
    explicit QgsWFSFeatureHitsAsyncRequest( QgsWFSDataSourceURI &uri );

    void launchGet( const QUrl &url );
    void launchPost( const QUrl &url, const QByteArray &data );

    //! Returns result of request, or -1 if not known/error
    int numberMatched() const { return mNumberMatched; }

  signals:
    void gotHitsResponse();

  private slots:
    void hitsReplyFinished();

  protected:
    QString errorMessageWithReason( const QString &reason ) override;

  private:
    int mNumberMatched;
};

/**
 * This class runs one (or several if paging is needed) GetFeature request,
 * process the results as soon as they arrived and notify them to the
 * serializer to fill the case, and to the iterator that subscribed
 * Instances of this class may be run in a dedicated thread (QgsWFSThreadedFeatureDownloader)
 * A progress dialog may pop-up in GUI mode (if the download takes a certain time)
 * to allow canceling the download.
*/
class QgsWFSFeatureDownloaderImpl final : public QgsWfsRequest, public QgsFeatureDownloaderImpl
{
    Q_OBJECT

    DEFINE_FEATURE_DOWNLOADER_IMPL_SLOTS // cppcheck-suppress duplInheritedMember

      signals :
      /* Used internally by the stop() method */
      void
      doStop();

    /* Emitted with the total accumulated number of features downloaded. */
    void updateProgress( long long totalFeatureCount );

  public:
    QgsWFSFeatureDownloaderImpl( QgsWFSSharedData *shared, QgsFeatureDownloader *downloader, bool requestMadeFromMainThread );
    ~QgsWFSFeatureDownloaderImpl() override;

    void run( bool serializeFeatures, long long maxFeatures ) override;

  protected:
    QString errorMessageWithReason( const QString &reason ) override;

  private slots:
    void startHitsRequest();
    void gotHitsResponse();

    void createProgressTask();

  private:
    QUrl buildURL( qint64 startIndex, long long maxFeatures, bool forHits );
    std::pair<QUrl, QByteArray> buildPostRequest( qint64 startIndex, long long maxFeatures, bool forHits );

    void pushError( const QString &errorMsg );
    QString sanitizeFilter( QString filter );
    std::pair<QString, QString> determineTypeNames() const;
    bool useInvertedAxis() const;

    //! Mutable data shared between provider, feature sources and downloader.
    QgsWFSSharedData *mShared = nullptr;

    long long mPageSize = 0;
    bool mRemoveNSPrefix = false;
    long long mNumberMatched = -1;
    QgsWFSFeatureHitsAsyncRequest mFeatureHitsAsyncRequest;
    qint64 mTotalDownloadedFeatureCount = 0;

    const QStringList WFS1FORMATS { QStringLiteral( "text/xml; subtype=gml/3.2.1" ), QStringLiteral( "application/gml+xml; version=3.2" ), QStringLiteral( "text/xml; subtype=gml/3.1.1" ), QStringLiteral( "application/gml+xml; version=3.1" ), QStringLiteral( "text/xml; subtype=gml/3.0.1" ), QStringLiteral( "application/gml+xml; version=3.0" ), QStringLiteral( "GML3" ) };
};


#endif // QGSWFSFEATUREITERATOR_H
