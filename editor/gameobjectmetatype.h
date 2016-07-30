#ifndef GAMEOBJECTMETATYPE_H
#define GAMEOBJECTMETATYPE_H

#include <QObject>
#include <QIcon>

class GameObjectMetaType : public QObject
{
    Q_OBJECT

public:

    enum Type {
        UnknownType=0,
        GameObject,
        //objects
        Object,
        TextBox,
        Button,
        Image,
        Character,
        ObjectGroup,
        DialogueBox,
        Menu,
        MenuOption,
        //scene
        Scene,
        //actions
        Action=64,
        Show,
        Hide,
        Wait,
        Dialogue,
        Slide,
        Fade,
        Label,
        GoToLabel,
        GoToScene,
        Branch,
        GetUserInput,
        ShowMenu,
        ChangeColor,
        PlaySound,
        StopSound,
        SetGameVariable,
        ChangeBackground,
        RunScript,
        ChangeState,
        End,
        firstObjectType = Object,
        lastObjectType = MenuOption,
        firstActionType = Action,
        lastActionType = End
    };

    GameObjectMetaType(Type, const QString&, const QString&, const QIcon& icon=QIcon(), QObject *parent = 0);
    const QIcon& icon() const;
    QString name() const;
    GameObjectMetaType::Type type() const;
    QString toString() const;

    static void init();
    static void destroy();
    static const GameObjectMetaType* metaType(Type);
    static GameObjectMetaType::Type typeFromString(const QString&);

signals:

public slots:

private:
    QIcon mIcon;
    QString mName;
    Type mType;
    QString mTypeString;

};

#endif // GAMEOBJECTMETATYPE_H
