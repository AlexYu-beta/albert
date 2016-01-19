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

#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <functional>
#include <vector>
#include <memory>
using std::shared_ptr;
#include <vector>
using std::vector;
#include "indexer.h"
#include "bookmark.h"
#include "extension.h"


/** ***************************************************************************/
void ChromeBookmarks::Indexer::run() {

    // Notification
    qDebug("[%s] Start indexing in background thread.", extension_->name);
    emit statusInfo("Indexing bookmarks ...");

    // Build a new index
    vector<shared_ptr<Bookmark>> newIndex;

    // Define a recursive bookmark indexing lambda
    std::function<void(const QJsonObject &json)> rec_bmsearch =
            [&rec_bmsearch, &newIndex](const QJsonObject &json) {
        QJsonValue type = json["type"];
        if (type == QJsonValue::Undefined)
            return;
        if (type.toString() == "folder"){
            QJsonArray jarr = json["children"].toArray();
            for (const QJsonValue &i : jarr)
                rec_bmsearch(i.toObject());
        }
        if (type.toString() == "url") {
            // TODO ADD THE FOLDERS to the aliases
            newIndex.push_back(std::make_shared<Bookmark>(
                                 json["name"].toString(), json["url"].toString(), 0));
        }
    };

    QFile f(extension_->bookmarksFile_);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open " << extension_->bookmarksFile_;
        return;
    }

    QJsonObject json = QJsonDocument::fromJson(f.readAll()).object();
    QJsonObject roots = json.value("roots").toObject();
    for (const QJsonValue &i : roots)
        if (i.isObject())
            rec_bmsearch(i.toObject());

    f.close();


    // Sort the new index for linear usage copy [O(n*log(n))]
    emit statusInfo("Sorting ... ");
    std::sort(newIndex.begin(), newIndex.end(),
              [&](const shared_ptr<Bookmark> lhs, const shared_ptr<Bookmark> rhs) {
                  return QString::compare(lhs->url(), rhs->url(), Qt::CaseInsensitive) < 0;
              });


    // Copy the usagecounters  [O(n)]
    emit statusInfo("Copy usage statistics ... ");
    size_t i=0, j=0;
    while (i < extension_->index_.size() && j < newIndex.size()) {
        if (extension_->index_[i]->url_ == newIndex[j]->url_) {
            newIndex[j]->usage_ = extension_->index_[i]->usage_;
            ++i;++j;
        } else if (extension_->index_[i]->url_ < newIndex[j]->url_ ) {
            ++i;
        } else {// if ((*_fileIndex)[i]->path > (*newIndex)[j]->path) {
            ++j;
        }
    }

    /*
     *  ▼ CRITICAL ▼
     */

    // Lock the access
    extension_->indexAccess_.lock();

    // Set the new index
    extension_->index_ = std::move(newIndex);

    // Reset the offline index
    emit statusInfo("Build offline index... ");
    extension_->searchIndex_.clear();

    // Build the new offline index
    for (shared_ptr<AlbertItem> i : extension_->index_)
        extension_->searchIndex_.add(i);

    // Unlock the accress
    extension_->indexAccess_.unlock();

    // Finally update the watches (maybe folders changed)
    extension_->watcher_.removePaths(extension_->watcher_.files());
    if(!extension_->watcher_.addPath(extension_->bookmarksFile_)) // No clue why this should happen
        qCritical() << extension_->bookmarksFile_
                    <<  "could not be watched. Changes in this path will not be noticed.";
    /*
     *  ▲ CRITICAL ▲
     */

    // Notification
    qDebug("[%s] Indexing done (%d items)", extension_->name, static_cast<int>(extension_->index_.size()));
    emit statusInfo(QString("Indexed %1 bookmarks").arg(extension_->index_.size()));
}
