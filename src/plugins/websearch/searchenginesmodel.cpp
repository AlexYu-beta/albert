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

#include "searchenginesmodel.h"
#include "searchengine.h"

/** ***************************************************************************/
int Websearch::SearchEnginesModel::rowCount(const QModelIndex &) const {
    return static_cast<int>(index_.size());
}



/** ***************************************************************************/
int Websearch::SearchEnginesModel::columnCount(const QModelIndex &) const {
    return static_cast<int>(Section::NumSections);
}



/** ***************************************************************************/
QVariant Websearch::SearchEnginesModel::headerData(int section, Qt::Orientation orientation, int role) const {
    // No sanity check necessary since
    if ( section<0 || static_cast<int>(Section::NumSections)<=section )
        return QVariant();


    if (orientation == Qt::Horizontal){
        switch (static_cast<Section>(section)) {
        case Section::Enabled:{
            switch (role) {
            case Qt::ToolTipRole: return "Enables the searchengine as fallback.";
            default: return QVariant();
            }
        }
        case Section::Name:{
            switch (role) {
            case Qt::DisplayRole: return "Name";
            case Qt::ToolTipRole: return "The name of the searchengine.";
            default: return QVariant();
            }

        }
        case Section::Trigger:{
            switch (role) {
            case Qt::DisplayRole: return "Trigger";
            case Qt::ToolTipRole: return "The term that triggers this searchengine.";
            default: return QVariant();
            }

        }
        case Section::URL:{
            switch (role) {
            case Qt::DisplayRole: return "URL";
            case Qt::ToolTipRole: return "The URL of this searchengine. %s will be replaced by your searchterm.";
            default: return QVariant();
            }

        }
        default: return QVariant();
        }
    }
    return QVariant();
}



/** ***************************************************************************/
QVariant Websearch::SearchEnginesModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()
            || index.row() >= static_cast<int>(index_.size())
            || index.column() >= static_cast<int>(Section::NumSections))
        return QVariant();

    switch (role) {
    case Qt::DisplayRole: {
        switch (static_cast<Section>(index.column())) {
        case Section::Name:  return index_[index.row()]->name();
        case Section::Trigger:  return index_[index.row()]->trigger();
        case Section::URL:  return index_[index.row()]->url();
        default: return QVariant();
        }
    }
    case Qt::EditRole: {
        switch (static_cast<Section>(index.column())) {
        case Section::Name:  return index_[index.row()]->name();
        case Section::Trigger:  return index_[index.row()]->trigger();
        case Section::URL:  return index_[index.row()]->url();
        default: return QVariant();
        }
    }
    case Qt::DecorationRole: {
        switch (static_cast<Section>(index.column())) {
        case Section::Name:  return index_[index.row()]->icon();
        default: return QVariant();
        }
    }
    case Qt::ToolTipRole: {
        switch (static_cast<Section>(index.column())) {
        case Section::Enabled:  return "Check to enable the search engine";
        default: return "Double click to edit";
        }
    }
    case Qt::CheckStateRole: {
        switch (static_cast<Section>(index.column())) {
        case Section::Enabled:  return (index_[index.row()]->enabled())?Qt::Checked:Qt::Unchecked;
        default: return QVariant();
        }
    }
    default:
        return QVariant();
    }
}



/** ***************************************************************************/
bool Websearch::SearchEnginesModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid()
            || index.row() >= static_cast<int>(index_.size())
            || index.column() >= static_cast<int>(Section::NumSections))
        return false;

    switch (role) {
    case Qt::DecorationRole: {
        index_[index.row()]->setIcon(value.toString());
        dataChanged(index, index, QVector<int>({Qt::DecorationRole}));
        return true;
    }
    case Qt::EditRole: {
        if ( !value.canConvert(QMetaType::QString) )
            return false;
        QString s = value.toString();
        switch (static_cast<Section>(index.column())) {
        case Section::Name:
            index_[index.row()]->setName(s);
            dataChanged(index, index, QVector<int>({Qt::DisplayRole}));
            return true;
        case Section::Trigger:
            index_[index.row()]->setTrigger(s);
            dataChanged(index, index, QVector<int>({Qt::DisplayRole}));
            return true;
        case Section::URL:
            index_[index.row()]->setUrl(s);
            dataChanged(index, index, QVector<int>({Qt::DisplayRole}));
            return true;
        default:
            return false;
        }
    }
    case Qt::CheckStateRole: {
        switch (static_cast<Section>(index.column())) {
        case Section::Enabled:
            index_[index.row()]->setEnabled(value.toBool());
            return true;
        default:
            return false;
        }
    }
    default:
        return false;
    }
    return false;
}



/** ***************************************************************************/
Qt::ItemFlags Websearch::SearchEnginesModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;

    switch (static_cast<Section>(index.column())) {
    case Section::Enabled:
        return Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsUserCheckable;
    default:
        return Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable;
    }
}



/** ***************************************************************************/
bool Websearch::SearchEnginesModel::insertRows(int position, int rows, const QModelIndex &) {
    if (position<0 || rows<1 || static_cast<int>(index_.size()<position))
        return false;

    beginInsertRows(QModelIndex(), position, position + rows - 1);
    for (int row = position; row < position + rows; ++row){
        index_.insert(index_.begin() + row,
                      std::make_shared<SearchEngine>(
                          "<name>",
                          "<http://url/containing/the/?searchexpression=%s>",
                          "<trigger>",
                          ":default",
                          false));
    }
    endInsertRows();
    return true;
}



/** ***************************************************************************/
bool Websearch::SearchEnginesModel::removeRows(int position, int rows, const QModelIndex &) {
    if (position<0 || rows<1 || static_cast<int>(index_.size()<position+rows))
        return false;

    beginRemoveRows(QModelIndex(), position, position + rows-1);
    index_.erase(index_.begin()+position,index_.begin()+(position+rows));
    endRemoveRows();
    return true;
}



/** ***************************************************************************/
bool Websearch::SearchEnginesModel::moveRows(const QModelIndex &src, int srcRow, int cnt, const QModelIndex &dst, int dstRow) {
    if (srcRow<0 || cnt<1 || dstRow<0
            || static_cast<int>(index_.size()<srcRow+cnt-1)
            || static_cast<int>(index_.size()<dstRow)
            || srcRow<=dstRow && dstRow<srcRow+cnt) // If its inside the source do nothing
        return false;

    std::vector<shared_ptr<SearchEngine>> tmp;
    beginMoveRows(src, srcRow, srcRow+cnt-1, dst, dstRow);
    tmp.insert(tmp.end(), make_move_iterator(index_.begin()+srcRow), make_move_iterator(index_.begin() + srcRow+cnt));
    index_.erase(index_.begin()+srcRow, index_.begin() + srcRow+cnt);
    const size_t finalDst = dstRow > srcRow ? dstRow - cnt : dstRow;
    index_.insert(index_.begin()+finalDst , make_move_iterator(tmp.begin()), make_move_iterator(tmp.end()));
    endMoveRows();
    return true;
}
