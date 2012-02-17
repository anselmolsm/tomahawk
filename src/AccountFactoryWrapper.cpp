/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Leo Franchi <lfranchi@kde.org>
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

#include "AccountFactoryWrapper.h"

#include "accounts/Account.h"
#include <accounts/AccountManager.h>
#include "AccountFactoryWrapperDelegate.h"
#include "delegateconfigwrapper.h"
#include "ui_AccountFactoryWrapper.h"

using namespace Tomahawk::Accounts;
AccountFactoryWrapper::AccountFactoryWrapper( AccountFactory* factory, QWidget* parent )
    : QDialog( parent, Qt::Sheet )
    , m_factory( factory )
    , m_ui( new Ui_AccountFactoryWrapper )
    , m_createAccount( false )
{
    m_ui->setupUi( this );

    setWindowTitle( factory->prettyName() );

    m_ui->factoryIcon->setPixmap( factory->icon() );
    m_ui->factoryDescription->setText( factory->description() );

    m_addButton = m_ui->buttonBox->addButton( tr( "Add Account" ), QDialogButtonBox::ActionRole );

    AccountFactoryWrapperDelegate* del = new AccountFactoryWrapperDelegate( m_ui->accountsList );
    m_ui->accountsList->setItemDelegate( del );

    connect( del, SIGNAL( openConfig( Tomahawk::Accounts::Account* ) ), this, SLOT( openAccountConfig( Tomahawk::Accounts::Account* ) ) );
    connect( del, SIGNAL( removeAccount( Tomahawk::Accounts::Account* ) ), this, SLOT( removeAccount( Tomahawk::Accounts::Account* ) ) );

    load();

    connect( m_ui->buttonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
    connect( m_ui->buttonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( m_ui->buttonBox, SIGNAL( clicked( QAbstractButton*) ), this, SLOT( buttonClicked( QAbstractButton* ) ) );


    connect ( AccountManager::instance(), SIGNAL( added( Tomahawk::Accounts::Account* ) ), this, SLOT( load() ) );
    connect ( AccountManager::instance(), SIGNAL( removed( Tomahawk::Accounts::Account* ) ), this, SLOT( load() ) );

#ifdef Q_OS_MAC
    setContentsMargins( 0, 0, 0, 0 );
    m_ui->verticalLayout->setSpacing( 6 );
#endif
}

void
AccountFactoryWrapper::load()
{
    m_ui->accountsList->clear();
    foreach ( Account* acc, AccountManager::instance()->accounts() )
    {
        if ( AccountManager::instance()->factoryForAccount( acc ) == m_factory )
        {
            QTreeWidgetItem* item = new QTreeWidgetItem( m_ui->accountsList );
            item->setData( 0, AccountRole, QVariant::fromValue< QObject *>( acc ) );
        }
    }

    if ( m_ui->accountsList->model()->rowCount() == 0 )
        accept();

#ifndef Q_OS_MAC
    const int padding = 7;
#else
    const int padding = 8;
#endif
    const int height = m_ui->accountsList->model()->rowCount( QModelIndex() ) * ACCOUNT_ROW_HEIGHT + padding;

    m_ui->accountsList->setFixedHeight( height );
}


void
AccountFactoryWrapper::openAccountConfig( Account* account )
{
    if( account->configurationWidget() )
    {
#ifndef Q_WS_MAC
        DelegateConfigWrapper dialog( account->configurationWidget(), QString("%1 Configuration" ).arg( account->accountFriendlyName() ), this );
        QWeakPointer< DelegateConfigWrapper > watcher( &dialog );
        int ret = dialog.exec();
        if( !watcher.isNull() && ret == QDialog::Accepted )
        {
            // send changed config to resolver
            account->saveConfig();
        }
#else
        // on osx a sheet needs to be non-modal
        DelegateConfigWrapper* dialog = new DelegateConfigWrapper( account->configurationWidget(), QString("%1 Configuration" ).arg( account->accountFriendlyName() ), this, Qt::Sheet );
        dialog->setProperty( "accountplugin", QVariant::fromValue< QObject* >( account ) );
        connect( dialog, SIGNAL( finished( int ) ), this, SLOT( accountConfigClosed( int ) ) );

        dialog->show();
#endif
    }
}


void
AccountFactoryWrapper::accountConfigClosed( int value )
{
    if( value == QDialog::Accepted )
    {
        DelegateConfigWrapper* dialog = qobject_cast< DelegateConfigWrapper* >( sender() );
        Account* account = qobject_cast< Account* >( dialog->property( "accountplugin" ).value< QObject* >() );
        account->saveConfig();
    }
}

void
AccountFactoryWrapper::removeAccount( Tomahawk::Accounts::Account* acct )
{
    AccountManager::instance()->removeAccount( acct );

    load();
}

void
AccountFactoryWrapper::buttonClicked( QAbstractButton* button )
{
    if ( button == m_addButton )
    {
        m_createAccount = true;
        emit createAccount( m_factory );
//         accept();
        return;
    }
    else
        reject();
}
