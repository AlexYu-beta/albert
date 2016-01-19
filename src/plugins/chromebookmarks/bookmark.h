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
#include "abstractobjects.hpp"

namespace ChromeBookmarks {

class Bookmark final : public AlbertItem
{
    friend class Extension;
    friend class Indexer;

public:
    Bookmark() = delete;
    Bookmark(const Bookmark &) = delete;
    Bookmark(const QString &name, const QString &url, short usage = 0)
        : name_(name), url_(url), usage_(usage) {}

    QString text() const override;
    QString subtext() const override;
    QUrl icon() const override;
    void activate() override;

    uint16_t usageCount() const override {return usage_;}
    vector<QString> aliases() const override;

    const QString &url() const {return url_;}

private:
    QString      name_;
    QString      url_;
    ushort       usage_;
};
}
