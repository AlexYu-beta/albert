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
#include <vector>
using std::vector;
#include "abstractobjects.hpp"

namespace Applications{

class DesktopAction;

class Application final : public AlbertItem
{
    friend class Extension;
    friend class Indexer;

public:
    Application() = delete;
    Application(const Application &) = delete;
    Application(const QString &path, short usage = 0)
        : path_(path), usage_(usage) {}

    QString text() const override;
    QString subtext() const override;
    QUrl icon() const override;
    uint16_t usageCount() const override {return usage_;}
    void activate() override;
    bool hasChildren() const override;
    vector<shared_ptr<AlbertItem>> children() override;
    vector<QString> aliases() const override;

    bool readDesktopEntry();
    const QString& path() const {return path_;}
    void incUsage() {++usage_;}

    static QString terminal;

private:
    static QString escapeString(const QString &unescaped);
//    static void parseCommandLine(const QString & cmdLine, QString *program, QStringList *arguments);

    QString path_;
    QString name_;
    QString altName_;
    QUrl    iconUrl_;
    QString exec_;
    bool    term_;
    mutable uint16_t usage_;
    vector<shared_ptr<AlbertItem>> actions_;
};
}
