/****************************************************************************
**
**  Copyright   : (C) 2012 Dismet S.A.S.
**              : Julian Salamanca Espinosa
**  e-mail      : julian.sse@gmail.com
**
*****************************************************************************
**
**  modulemngr.cpp is part of Sipred.
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

#include "modulemngr.h"
#include "modulemngr_p.h"
#include "moduleinfo.h"

#include <QDebug>

ModuleMngrPrivate::ModuleMngrPrivate(ModuleMngr *q) :
    q_ptr(q)
{
    this->initModuleManager();
    this->loadModules();
}

ModuleMngr::ModuleMngr(QObject *parent) :
    QObject(parent), d_ptr(new ModuleMngrPrivate(this))
{

}

void ModuleMngrPrivate::initModuleManager()
{
    this->setModulesPath("../lib/modules");
    qDebug() << "Modules path is:" << m_modulesDir.path();
}

void ModuleMngrPrivate::loadModules()
{
    qDebug() << "Loading modules...";

    foreach (QString moduleDir, m_modulesDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        m_modulesDir.cd(moduleDir);

        foreach (QString fileName, m_modulesDir.entryList(QDir::Files)) {
            if (!QLibrary::isLibrary(fileName))
                continue;

            QString filePath = m_modulesDir.absoluteFilePath(fileName);

            m_loader.setFileName(filePath);

            QObject *moduleInstance = m_loader.instance();
            if (!moduleInstance)
                continue;

            ModuleFactory *moduleFactory = qobject_cast<ModuleFactory *>(moduleInstance);

            if (!moduleFactory)
                continue;

            Module *module = moduleFactory->module();

            if (!module)
                continue;

            this->registerModule(module, fileName);

            m_loader.unload();
        }

        m_modulesDir.cdUp();
    }
}

bool ModuleMngrPrivate::setModulesPath(const QString &path)
{
    QDir dir(path);

    if (!dir.exists()) {
        qWarning() << "Path " << path << " doesn't exist";
        return false;
    }

    if (dir.isRelative())
        qDebug() << "Set a relative path:" << path;

    m_modulesDir.setPath(path);

    return true;
}

void ModuleMngrPrivate::registerModule(Module *module, const QString &fileName)
{
    Q_Q(ModuleMngr);

    if (m_modulesInfo.contains(module->id())) {
        qWarning() << "Module ID:" << module->id() << "already exist";
        return;
    }

    ModuleInfo info(q);
    info.setId(module->id());
    info.setFileName(fileName);
    info.setName(module->name());
    info.setVersion(module->version());
    info.setSummary(module->summary());
    info.setCategory(module->category());
    info.setAuthor(module->author());
    info.setMail(module->mail());
    info.setWebside(module->webside());
    info.setLicence(module->license());
    info.setIcon(module->icon());
    info.setInstance(module->instance());
    info.setConfigurable(module->configurable());

    m_modulesInfo.insert(info.id(), info);
}
