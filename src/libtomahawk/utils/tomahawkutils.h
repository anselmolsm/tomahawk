/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Christian Muehlhaeuser <muesli@tomahawk-player.org>
 *
 *   Tomahawk is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Tomahawk is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Tomahawk. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TOMAHAWKUTILS_H
#define TOMAHAWKUTILS_H

#include "dllmacro.h"

#include <QtCore/QThread>
#include <QtNetwork/QNetworkProxy>
#include <QtCore/QStringList>
#include <typedefs.h>


#define RESPATH ":/data/"


class QDir;
class QNetworkAccessManager;

namespace TomahawkUtils
{
    enum MediaType
    {
        MediaTypeArtist,
        MediaTypeAlbum,
        MediaTypeTrack
    };

    enum ImageType
    {
        DefaultAlbumCover,
        DefaultArtistImage
    };
    enum ImageMode
    {
        NoDefaultCover,
        CoverInCase,
        ScaledCover
    };

    class DLLEXPORT NetworkProxyFactory : public QNetworkProxyFactory
    {
    public:
        NetworkProxyFactory()
            : m_proxy( QNetworkProxy::NoProxy )
            {}

        NetworkProxyFactory( const NetworkProxyFactory &other );
        virtual ~NetworkProxyFactory() {}

        virtual QList< QNetworkProxy > queryProxy( const QNetworkProxyQuery & query = QNetworkProxyQuery() );

        virtual void setNoProxyHosts( const QStringList &hosts );
        virtual QStringList noProxyHosts() const { return m_noProxyHosts; }
        virtual void setProxy( const QNetworkProxy &proxy );
        virtual QNetworkProxy proxy() { return m_proxy; }

        virtual NetworkProxyFactory& operator=( const NetworkProxyFactory &rhs );
        virtual bool operator==( const NetworkProxyFactory &other ) const;

    private:
        QStringList m_noProxyHosts;
        QNetworkProxy m_proxy;
    };

    DLLEXPORT QString appFriendlyVersion();

    DLLEXPORT QDir appConfigDir();
    DLLEXPORT QDir appDataDir();
    DLLEXPORT QDir appLogDir();

    DLLEXPORT QString sqlEscape( QString sql );
    DLLEXPORT QString timeToString( int seconds );
    DLLEXPORT QString ageToString( const QDateTime& time, bool appendAgoString = false );
    DLLEXPORT QString filesizeToString( unsigned int size );
    DLLEXPORT QString extensionToMimetype( const QString& extension );
    DLLEXPORT bool newerVersion( const QString& oldVersion, const QString& newVersion );

    DLLEXPORT NetworkProxyFactory* proxyFactory( bool noMutexLocker = false );
    DLLEXPORT void setProxyFactory( TomahawkUtils::NetworkProxyFactory* factory, bool noMutexLocker = false );
    DLLEXPORT QNetworkAccessManager* nam();
    DLLEXPORT void setNam( QNetworkAccessManager* nam, bool noMutexLocker = false );
    DLLEXPORT quint64 infosystemRequestId();

    DLLEXPORT QString md5( const QByteArray& data );
    DLLEXPORT bool removeDirectory( const QString& dir );

    /**
     * This helper is designed to help "update" an existing playlist with a newer revision of itself.
     * To avoid re-loading the whole playlist and re-resolving tracks that are the same in the old playlist,
     * it goes through the new playlist and adds only new tracks.
     *
     * The new list of tracks is returned
     *
     * \return true if some changes were made, false if the new tracks are the same as the current tracks in \param orig
     */
    DLLEXPORT QList< Tomahawk::query_ptr > mergePlaylistChanges( const QList< Tomahawk::query_ptr >& orig, const QList< Tomahawk::query_ptr >& newTracks, bool& changed );

    DLLEXPORT void crash();
}

#endif // TOMAHAWKUTILS_H
