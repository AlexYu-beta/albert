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
#include <QClipboard>
#include <QSettings>
#include "albertapp.h"
#include "extension.h"
#include "configwidget.h"
#include "query.h"
#include "objects.hpp"
#include "iconlookup/xdgiconlookup.h"

const QString Calculator::Extension::CFG_SEPS      = "group_separators";
const bool    Calculator::Extension::CFG_SEPS_DEF  = false;

/** ***************************************************************************/
Calculator::Extension::Extension() : IExtension("Calculator") {
    qDebug("[%s] Initialize extension", name);

    // Load settings
    QSettings s;
    s.beginGroup(name);
    loc_.setNumberOptions(
                (s.value(CFG_SEPS, CFG_SEPS_DEF).toBool())
                ? loc_.numberOptions() & ~QLocale::OmitGroupSeparator
                : loc_.numberOptions() | QLocale::OmitGroupSeparator );

    // Init icon
    XdgIconLookup xdg;
    QString iconPath = xdg.lookupIcon("calc");
    iconUrl_ = iconPath.isNull()
            ? QUrl("qrc:calc")
            : QUrl::fromLocalFile(iconPath);

    // Init parser
    parser_.reset(new mu::Parser);
    parser_->SetDecSep(loc_.decimalPoint().toLatin1());
    parser_->SetThousandsSep(loc_.groupSeparator().toLatin1());

    qDebug("[%s] Extension initialized", name);
}



/** ***************************************************************************/
Calculator::Extension::~Extension() {
    qDebug("[%s] Finalize extension", name);

    parser_.reset();

    // Save settings
    QSettings s;
    s.beginGroup(name);
    s.setValue(CFG_SEPS, !loc_.numberOptions().testFlag(QLocale::OmitGroupSeparator));

    qDebug("[%s] Extension finalized", name);
}



/** ***************************************************************************/
QWidget *Calculator::Extension::widget(QWidget *parent) {
    if (widget_.isNull()){
        widget_ = new ConfigWidget(parent);

        widget_->ui.checkBox_groupsep->setChecked(!(loc_.numberOptions() & QLocale::OmitGroupSeparator));
        connect(widget_->ui.checkBox_groupsep, &QCheckBox::toggled, [this](bool checked){
            this->loc_.setNumberOptions( (checked) ? this->loc_.numberOptions() & ~QLocale::OmitGroupSeparator
                                                  : this->loc_.numberOptions() | QLocale::OmitGroupSeparator );
        });
    }
    return widget_;
}



/** ***************************************************************************/
void Calculator::Extension::handleQuery(shared_ptr<Query> query) {
    parser_->SetExpr(query->searchTerm().toStdString());
    QString result;
    try {
        result = loc_.toString(parser_->Eval(), 'G', 16);
    }
    catch (mu::Parser::exception_type &e) {
      return;
    }

    std::shared_ptr<StandardItem> calcItem = std::make_shared<StandardItem>();
    calcItem->setText(result);
    calcItem->setSubtext(QString("Result of '%1'").arg(query->searchTerm()));
    calcItem->setIcon(iconUrl_);
    calcItem->setAction([result](){
        QApplication::clipboard()->setText(result);
        qApp->hideMainWindow();
    });
    query->addMatch(calcItem, SHRT_MAX);
}
