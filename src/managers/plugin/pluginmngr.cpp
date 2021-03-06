/****************************************************************************
**
**  Copyright   : (C) 2012 Dismet S.A.S.
**              : Julian Salamanca Espinosa
**  e-mail      : julian.sse@gmail.com
**
*****************************************************************************
**
**  pluginmngr.cpp is part of Sipred.
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

#include "pluginmngr.h"
#include "pluginmngr_p.h"

#include <QHash>
#include <QDebug>

PluginMngr::PluginMngr(QObject *parent) :
    QObject(parent), d_ptr(new PluginMngrPrivate(this))
{

}

PluginMngr::~PluginMngr()
{
    Q_D(PluginMngr);

    foreach (QString pluginId, d->m_activePlugins.keys()) {
        d->unloadPlugin(pluginId);
    }
}

bool PluginMngr::activePlugin(const QString &pluginId)
{
    Q_D(PluginMngr);

    if (!d->m_pluginsInfo.contains(pluginId)) {
        qWarning() << "Plugin" << pluginId << "can't be activated.";
        return false;
    }

    return false;
}

bool PluginMngr::disablePlugin(const QString &pluginId)
{
    Q_UNUSED(pluginId);
    return true;
}

PluginInfo PluginMngr::pluginInfo(const QString &pluginId)
{
    Q_D(PluginMngr);

    if (!d->m_pluginsInfo.contains(pluginId)) {
        qWarning() << "Requested plugin id:" << pluginId
                   << "is not avaliable.";
        return PluginInfo();
    }

    return d->m_pluginsInfo.value(pluginId, PluginInfo());
}

QStringList PluginMngr::avaliablePlugins()
{
    Q_D(PluginMngr);

    QStringList list = d->m_pluginsInfo.keys();

    return list;
}

Plugin* PluginMngr::plugin(const QString &pluginId)
{
    Q_D(PluginMngr);

    if (d->m_activePlugins.contains(pluginId))
        return d->m_activePlugins.value(pluginId);

    if (!d->m_pluginsInfo.contains(pluginId)) {
        qWarning() << "Requested plugin id:" << pluginId
                   << "is not avaliable.";
        return NULL;
    }

    if (d->loadPlugin(pluginId))
        return d->m_activePlugins.value(pluginId);

    return NULL;
}


/***************************************************************************
 *                           PluginMngrPrivate
 ***************************************************************************/

PluginMngrPrivate::PluginMngrPrivate(PluginMngr *parent) :
    q_ptr(parent)
{
    initPluginManager();
    loadPlugins();
}

PluginMngrPrivate::~PluginMngrPrivate()
{

}

void PluginMngrPrivate::initPluginManager()
{
    setPluginsPath("../lib/plugins");
}

void PluginMngrPrivate::loadPlugins()
{
    qDebug() << "Loading plugins...";

    foreach (QString pluginDir, m_pluginsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        m_pluginsDir.cd(pluginDir);

        qDebug() << m_pluginsDir.absolutePath();
        qDebug() << m_pluginsDir.entryList(QDir::Files);
        foreach (QString fileName, m_pluginsDir.entryList(QDir::Files)) {
            if(!QLibrary::isLibrary(fileName))
                continue;

            QString filePath = m_pluginsDir.absoluteFilePath(fileName);
            m_loader.setFileName(filePath);

            if (!m_loader.load()) {
                qWarning() << "Error cargando el plugin:" << fileName;
                qWarning() << m_loader.errorString();
                continue;
            }

            QObject *pluginInstance = m_loader.instance();
            if (!pluginInstance)
                continue;

            PluginFactory *pluginFactory = qobject_cast<PluginFactory *>(pluginInstance);

            if (!pluginFactory)
                continue;

            Plugin *plugin = pluginFactory->plugin();

            if (!plugin)
                continue;

            this->registerPlugin(plugin, filePath);

            m_loader.unload();
        }

        m_pluginsDir.cdUp();
    }
}

bool PluginMngrPrivate::setPluginsPath(const QString &path)
{
    QDir dir(path);

    if (!dir.exists()) {
        qWarning() << "Plugin path" << path << "doesn't exist";
        return false;
    }

    if (dir.isRelative())
        qWarning() << "Set a relative path:" << path;

    m_pluginsDir.setPath(path);

    return true;
}

void PluginMngrPrivate::registerPlugin(Plugin *plugin,
                                       const QString &fileName)
{
    if (m_pluginsInfo.contains(plugin->id())) {
        qWarning() << "Plugin ID:" << plugin->id() << "already exist.";
        return;
    }

    PluginInfo info;
    info.setId(plugin->id());
    info.setFileName(fileName);
    info.setName(plugin->name());
    info.setVersion(plugin->version());
    info.setSummary(plugin->summary());
    info.setCategory(plugin->category());
    info.setAuthor(plugin->author());
    info.setMail(plugin->mail());
    info.setWebside(plugin->webside());
    info.setLicence(plugin->licence());
    info.setIcon(plugin->icon());
    info.setConfigurable(plugin->isConfigurable());
    info.setConfigList(plugin->configList());

    m_pluginsInfo.insert(info.id(), info);

    qDebug() << "Register plugin:";
//    qDebug() << info;
}

bool PluginMngrPrivate::loadPlugin(const QString &pluginId)
{
    Q_Q(PluginMngr);

    if (!m_pluginsInfo.contains(pluginId)) {
        qWarning() << "Requested plugin id:" << pluginId << endl
                   << "is not avaliable.";
        return false;
    }

    if (m_activePlugins.contains(pluginId)) {
        qWarning() << "Requested plugin id:" << pluginId << endl
                   << "is already loaded.";
        return false;
    }

    m_loader.setFileName(m_pluginsInfo.value(pluginId).fileName());

    QObject *pluginInstance = m_loader.instance();

    if (!pluginInstance)
        return false;

    PluginFactory *pluginFactory = qobject_cast<PluginFactory *>(pluginInstance);

    if (!pluginInstance)
        return false;

    Plugin *plugin = pluginFactory->plugin();

    if (!plugin)
        return false;

    plugin->registerPluginManager(q);
    plugin->start();

    m_activePlugins.insert(pluginId, plugin);

    emit q->pluginLoaded();
    emit q->pluginLoaded(pluginId);
    emit q->configChange();

    return true;
}

bool PluginMngrPrivate::unloadPlugin(const QString &pluginId)
{
    if (!m_activePlugins.contains(pluginId)) {
        qWarning() << "Requested plugin id:" << pluginId << endl
                   << "is not loaded.";
        return false;
    }

    m_activePlugins.value(pluginId)->stop();
//    delete m_activePlugins.value(pluginId);
    m_activePlugins.remove(pluginId);

    m_loader.setFileName(m_pluginsInfo.value(pluginId).fileName());

    if (!m_loader.unload()) {
        qWarning() << "Can't unload plugin id:" << pluginId << endl
                   << m_loader.errorString();
        return false;
    }

    return true;
}
