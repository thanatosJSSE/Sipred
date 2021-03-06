/****************************************************************************
**
**  Copyright   : (C) 2012 Dismet S.A.S.
**              : Julian Salamanca Espinosa
**  e-mail      : julian.sse@gmail.com
**
*****************************************************************************
**
**  interfacemngr.cpp is part of Sipred.
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

#include "interfacemngr.h"
#include "interfacemngr_p.h"

#include "pluginmngr.h"
#include "modulemngr.h"

#include <QtUiTools>
#include <QScreen>

#include <QDebug>

InterfaceMngr::InterfaceMngr(QObject *parent) :
    QObject(parent), d_ptr(new InterfaceMngrPrivate(this))
{
}

InterfaceMngr::~InterfaceMngr()
{

}

void InterfaceMngr::createConnections()
{
}

void InterfaceMngr::registerModuleManager(ModuleMngr *moduleMngr)
{
    Q_D(InterfaceMngr);

    d->m_moduleManager = moduleMngr;

    qDebug() << d->m_moduleManager->avaliableModules(Module::ModuleTypeCore);

    foreach (QString moduleId, d->m_moduleManager->avaliableModules()) {
        Module *module = d->m_moduleManager->module(moduleId);
        if (module->menu())
            d->m_mainwindow->menuBar()->addMenu(module->menu());

        if (module->centralWidget()) {
            d->m_centralWidget->addWidget(module->centralWidget());
        }

        if (module->controlsWidget()) {
            QToolBox *toolBox = d->m_dockWidget->findChild<QToolBox *>();
            if (toolBox) {
                if (toolBox->count() == 1 &&
                    toolBox->itemText(toolBox->currentIndex()).isEmpty()) {
                    toolBox->addItem(module->controlsWidget(),
                                     module->icon(),
                                     module->name());
                    toolBox->removeItem(0);
                } else {
                    toolBox->addItem(module->controlsWidget(),
                                     module->icon(),
                                     module->name());
                }
            }
        }
    }
}

void InterfaceMngr::registerPluginManager(PluginMngr *pluginMngr)
{
    Q_D(InterfaceMngr);

    d->m_pluginManager = pluginMngr;

    foreach (QString pluginId, d->m_pluginManager->avaliablePlugins()) {
        Plugin *p = d->m_pluginManager->plugin(pluginId);
        d->m_mainwindow->menuBar()->addMenu(p->menu());
    }
}

void InterfaceMngr::initInterface()
{
    Q_D(InterfaceMngr);

    d->loadMainwindow();

    if (!d->m_mainwindow)
        d->setDefaultWindow();

    d->centerWindow();

    d->setDefaultConfigWidget();

    d->loadDockWidget();

    d->setDefaultCenterWidget();

    if (!d->m_dockWidget)
        d->setDefaultDock();

    d->m_mainwindow->addDockWidget(Qt::LeftDockWidgetArea, d->m_dockWidget);

    d->initializeMenus();

    d->m_mainwindow->show();
}

void InterfaceMngr::endInterface()
{

}

void InterfaceMngr::newProject()
{

}

bool InterfaceMngr::openProject()
{

}

bool InterfaceMngr::saveProject()
{

}

bool InterfaceMngr::saveAsProject()
{

}

void InterfaceMngr::quitApp()
{

}

/****************************************************************************
 *                          InterfaceMngrPrivate
 ***************************************************************************/

InterfaceMngrPrivate::InterfaceMngrPrivate(InterfaceMngr *q) :
    q_ptr(q), m_mainwindow(0)
{
    m_mainwindow = NULL;
    m_moduleManager = NULL;
    m_pluginManager = NULL;
    m_menuBar = NULL;
    m_toolBar = NULL;
    m_centralWidget = NULL;
    m_dockWidget = NULL;
    m_treeInformation = NULL;
}

void InterfaceMngrPrivate::loadMainwindow()
{
    QFile file("../share/mainwindow.ui");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Can't open mainwindow.ui";
        qWarning() << file.errorString();
        return;
    }

    QUiLoader uiLoader;

    m_mainwindow = qobject_cast<QMainWindow *>(uiLoader.load(&file));

    file.close();

    if (!m_mainwindow)
        qWarning() << "Can't load mainwindow.ui";
}

void InterfaceMngrPrivate::loadDockWidget()
{
    QFile file("../share/dockwidget.ui");

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Can't open dockwidget.ui";
        qWarning() << file.errorString();
        return;
    }

    QUiLoader loader;

    m_dockWidget = qobject_cast<QDockWidget *>(loader.load(&file));

    file.close();

    if (!m_dockWidget)
        qWarning() << "Can't load dockwidget.ui";
}

void InterfaceMngrPrivate::setDefaultWindow()
{
    QFile file(":/ui/mainwindow");

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning(file.errorString().toLatin1());
        qFatal("Can't open default mainwindow.ui");
    }

    QUiLoader loader;

    m_mainwindow = qobject_cast<QMainWindow *>(loader.load(&file));

    file.close();

    if (!m_mainwindow) {
        qWarning() << file.errorString().toLatin1();
        qFatal("Can't load default mainwindow.");
    }

    QIcon icon(":/thumbnail/logo_sipred");
    m_mainwindow->setWindowIcon(icon);
    setWindowTitle("Dismet");
}

void InterfaceMngrPrivate::setDefaultDock()
{
    QFile file(":/ui/dockwidget");

    if (!file.open(QIODevice::ReadOnly))
        qFatal("Can't open default dockwidget.ui");

    QUiLoader loader;

    m_dockWidget = qobject_cast<QDockWidget *>(loader.load(&file, m_mainwindow));

    if (!m_dockWidget)
        qFatal("Can't load default dockwidget");

    QToolBox *toolBox = m_dockWidget->findChild<QToolBox *>();

    if (!toolBox)
        qFatal("Can't find tool box.");

    QObject::connect(toolBox, SIGNAL(currentChanged(int)), m_centralWidget, SLOT(setCurrentIndex(int)));
    toolBox->setCurrentIndex(0);

    m_dockWidget->setWindowTitle("Herramientas");
}

void InterfaceMngrPrivate::setWindowTitle(QString title)
{
    if (!m_mainwindow)
        return;

    if (title.isEmpty())
        title.append("Sipred");
    else
        title.append(" - Sipred");

    m_mainwindow->setWindowTitle(title);
}

void InterfaceMngrPrivate::setDefaultCenterWidget()
{
    m_centralWidget = new QStackedWidget(m_mainwindow);
    m_mainwindow->setCentralWidget(m_centralWidget);
}

void InterfaceMngrPrivate::setDefaultConfigWidget()
{

}

void InterfaceMngrPrivate::initializeMenus()
{
    Q_Q(InterfaceMngr);

    if (!m_mainwindow)
        qFatal("Can't init actions, Mainwindow is not found");

    QList<QAction *> actionList;

    m_newAction = new QAction(m_mainwindow);
    m_newAction->setText("&Nuevo");
    m_newAction->setShortcut(QKeySequence::New);
    m_newAction->setToolTip("Crear un nuevo proyecto desde el gestor.");
    m_newAction->setStatusTip("Crear nuevo proyecto");

    QObject::connect(m_newAction, SIGNAL(triggered()), q, SLOT(newProject()));
    actionList.append(m_newAction);

    m_openAction = new QAction(m_mainwindow);
    m_openAction->setText("&Abrir");
    m_openAction->setShortcut(QKeySequence::Open);
    m_openAction->setToolTip("Abrir archivo soportado");
    m_openAction->setStatusTip("Abriendo archivo...");

    QObject::connect(m_openAction, SIGNAL(triggered()), q, SLOT(openProject()));
    actionList.append(m_openAction);

    m_saveAction = new QAction(m_mainwindow);
    m_saveAction->setText("&Guardar");
    m_saveAction->setShortcut(QKeySequence::Save);
    m_saveAction->setToolTip("Guardar el actual proyecto");
    m_saveAction->setStatusTip("Guardando archivo...");

    QObject::connect(m_saveAction, SIGNAL(triggered()), q, SLOT(saveProject()));
    actionList.append(m_saveAction);

    m_saveAsAction = new QAction(m_mainwindow);
    m_saveAsAction->setText("Guardar como");
    m_saveAsAction->setShortcut(QKeySequence::SaveAs);
    m_saveAsAction->setStatusTip("Guardando el proyecto como...");

    QObject::connect(m_saveAsAction, SIGNAL(triggered()), q, SLOT(saveAsProject()));
    actionList.append(m_saveAsAction);

    m_quitAction = new QAction(m_mainwindow);
    m_quitAction->setText("&Salir");
    m_quitAction->setShortcut(QKeySequence::Quit);
    m_quitAction->setToolTip("Salir de la aplicación");
    m_quitAction->setStatusTip("Salir");

    QObject::connect(m_quitAction, SIGNAL(triggered()), q, SLOT(quitApp()));

    m_fileMenu = m_mainwindow->menuBar()->addMenu("&Archivo");
    m_fileMenu->addActions(actionList);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_quitAction);

    m_configAction = new QAction(m_mainwindow);
    m_configAction->setText("&Configuración");
    m_configAction->setToolTip("Configurar componentes de Sipred");
    m_configAction->setStatusTip("Configurar Sipred");

//    QObject::connect(m_configAction, SIGNAL(triggered()), q, SLOT(startConfigDialog()));
}

void InterfaceMngrPrivate::centerWindow()
{
    QSize dSize = qApp->desktop()->size();

    qint32 w = dSize.width();
    qint32 h = dSize.height();

    w *= 0.7;
    h *= 0.7;

    QSize newSize(w, h);

    m_mainwindow->setGeometry(QStyle::alignedRect(Qt::LeftToRight,
                                                  Qt::AlignCenter,
                                                  newSize,
                                                  qApp->desktop()->availableGeometry()));
}
