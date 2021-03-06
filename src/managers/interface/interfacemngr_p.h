/****************************************************************************
**
**  Copyright   : (C) 2012 Dismet S.A.S.
**              : Julian Salamanca Espinosa
**  e-mail      : julian.sse@gmail.com
**
*****************************************************************************
**
**  interfacemngr_p.h is part of Sipred.
**
**    Sipred is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**    Sipred is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with Sipred. If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef INTERFACEMNGR_P_H
#define INTERFACEMNGR_P_H

#include <QtCore/QtGlobal>
#include <QtGui>

class InterfaceMngr;
class ModuleMngr;
class PluginMngr;

class InterfaceMngrPrivate
{
    Q_DECLARE_PUBLIC(InterfaceMngr)

public:
    InterfaceMngrPrivate(InterfaceMngr *q);
    void loadMainwindow();
    void loadDockWidget();
    void setDefaultWindow();

    void setWindowTitle(QString title = QString());

//    void addConfigPage(QWidget *);

    InterfaceMngr * const q_ptr;

    QMainWindow *m_mainwindow;
    ModuleMngr *m_moduleManager;
    PluginMngr *m_pluginManager;

private:
    void setDefaultDock();
    void setDefaultCenterWidget();
    void setDefaultConfigWidget();

    void initializeMenus();
    void centerWindow();

    QMenuBar *m_menuBar;
    QToolBar *m_toolBar;
    QStackedWidget *m_centralWidget;
    QDockWidget *m_dockWidget;
    QMap<QString, QVariant> *m_treeInformation;

    /* Actions */
    /* File Menu */
    QMenu *m_fileMenu;
    QAction *m_newAction;
    QAction *m_openAction;
    QAction *m_saveAction;
    QAction *m_saveAsAction;
    QAction *m_quitAction;

    /* Tools Menu */
    QAction *m_configAction;

};

#endif // INTERFACEMNGR_P_H
