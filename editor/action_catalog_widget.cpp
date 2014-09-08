/* Copyright (C) 2012-2014 Carlos Pais
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "action_catalog_widget.h"

#include <QDebug>

#include "wait.h"
#include "dialogue.h"
#include "label.h"
#include "gotolabel.h"
#include "gotoscene.h"
#include "slide.h"
#include "fade.h"
#include "show.h"
#include "hide.h"
#include "branch.h"
#include "get_user_input.h"
#include "changecolor.h"
#include "playsound.h"
#include "stopsound.h"
#include "show_menu.h"
#include "end.h"
#include "change_game_variable.h"
#include "change_background.h"
#include "runscript.h"
#include "changestate.h"

ActionCatalogWidget::ActionCatalogWidget(QWidget *parent) :
    PropertiesWidget(parent, 1)
{
    this->setHeaderHidden(true);

    beginGroup(tr("Story"));
    appendRow(Dialogue::Info.icon, Dialogue::Info.name);
    appendRow(Wait::Info.icon, Wait::Info.name);
    appendRow(Show::Info.icon, Show::Info.name);
    appendRow(Hide::Info.icon, Hide::Info.name);
    appendRow(ChangeBackground::Info.icon, ChangeBackground::Info.name);
    appendRow(ChangeState::Info.icon, ChangeState::Info.name);
    appendRow(Label::Info.icon, Label::Info.name);
    appendRow(GoToLabel::Info.icon, GoToLabel::Info.name);
    appendRow(GoToScene::Info.icon, GoToScene::Info.name);
    appendRow(Branch::Info.icon, Branch::Info.name);
    appendRow(ShowMenu::Info.icon, ShowMenu::Info.name);
    appendRow(End::Info.icon, End::Info.name);
    endGroup();

    beginGroup(tr("Transform"));
    appendRow(Slide::Info.icon, Slide::Info.name);
    appendRow(Fade::Info.icon, Fade::Info.name);
    appendRow(ChangeColor::Info.icon, ChangeColor::Info.name);
    endGroup();

    beginGroup(tr("Audio"));
    appendRow(PlaySound::Info.icon, PlaySound::Info.name);
    appendRow(StopSound::Info.icon, StopSound::Info.name);
    endGroup();

    beginGroup(tr("Scripting"));
    appendRow(GetUserInput::Info.icon, GetUserInput::Info.name);
    appendRow(ChangeGameVariable::Info.icon, ChangeGameVariable::Info.name);
    appendRow(RunScript::Info.icon, RunScript::Info.name);
    endGroup();

    setIconSize(QSize(22, 22));

    connect(this, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(onDoubleClick(const QModelIndex&)));
}


void ActionCatalogWidget::onDoubleClick(const QModelIndex & index)
{
    if (! index.parent().isValid())
        return;

    const QStandardItemModel* model = qobject_cast<const QStandardItemModel*> (index.model());
    if (! model)
        return;

    int row = index.parent().row();
    int childRow = index.row();

    for (int i=0; i < row; i++ ) {
        if (model->item(i, 0))
            childRow += model->item(i)->rowCount();
    }

    Action *action = 0;

    switch (childRow) {
    case Actions::Dialogue: action = new Dialogue(); break;
    case Actions::Wait: action = new Wait(); break;
    case Actions::Show: action = new Show(); break;
    case Actions::Hide: action = new Hide(); break;
    case Actions::ChangeBackground: action = new ChangeBackground(); break;
    case Actions::ChangeState: action = new ChangeState(); break;
    case Actions::Label: action = new Label(""); break;
    case Actions::GoToLabel: action = new GoToLabel(""); break;
    case Actions::GoToScene: action = new GoToScene(); break;
    case Actions::ShowMenu: action = new ShowMenu(); break;
    case Actions::Slide: action = new Slide(); break;
    case Actions::Fade: action = new Fade(); break;
    case Actions::Branch: action = new Branch(); break;
    case Actions::ChangeColor: action = new ChangeColor(); break;
    case Actions::PlaySound: action = new PlaySound(); break;
    case Actions::StopSound: action = new StopSound(); break;
    case Actions::End: action = new End(); break;
    case Actions::GetUserInput: action = new GetUserInput(); break;
    case Actions::ChangeGameVariable: action = new ChangeGameVariable(); break;
    case Actions::RunScript: action = new RunScript(); break;
    }

    if (action)
        emit newAction(action);
}
