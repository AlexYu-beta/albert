// albert - a simple application launcher for linux
// Copyright (C) 2014-2015 Manuel Schneider
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once
#include <QAbstractTableModel>
#include <QFileSystemWatcher>
#include <QPointer>
#include <QIcon>
#include <memory>
#include "iextension.h"
#include "search/search.h"

namespace Websearch {

class SearchEngine;
class ConfigWidget;

class Extension final : public QAbstractTableModel, public IExtension
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ALBERT_EXTENSION_IID FILE "metadata.json")
    Q_INTERFACES(IExtension)

    enum class Section{Enabled, Name, Trigger, URL, Count};

public:
    Extension();
    ~Extension();

    // GenericPluginInterface
    QWidget *widget(QWidget *parent = nullptr) override;

    // IExtension
    void handleQuery(shared_ptr<Query> query) override;
    void handleFallbackQuery(shared_ptr<Query> query) override;
    bool isTriggerOnly() const override {return true;}
    virtual bool runExclusive() const {return true;}
    QStringList triggers() const override;

    // API special to this extension
    void restoreDefaults();

    // Modelinterface
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex & index) const override;
    bool insertRows (int position, int rows, const QModelIndex & parent = QModelIndex()) override;
    bool removeRows (int position, int rows, const QModelIndex & parent = QModelIndex()) override;
    bool moveRows(const QModelIndex &sourceRow, int srcRow, int cnt, const QModelIndex & dst, int destinationChild) override;

private:
    QPointer<ConfigWidget> widget_;
    std::vector<shared_ptr<SearchEngine>> index_;

    /* const */
    static const QString EXT_NAME;
};

}
