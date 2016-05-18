#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QVariantList>
#include <QMap>
#include <QUuid>
#include "../common/nutmegobject.h"

class Task
{
public:
    enum Command {
        Undefined=-1,

        SetFigure=0,
        SetProperty=1,
        GetProperty=2,
        Invoke=3,

        SetGui=10,
        SetGuiProperty=11,

        AddComponent=20
    };

    // ---------------- Constructors ----------------- //
    Task(Command cmd, const QString &tar, const QVariantList &arg, const QUuid &session, uint64_t id=0)
        : command(cmd)
        , commandString(commandToString(cmd))
        , target(tar)
        , targetObject(0)
        , targetName(QString())
        , args(arg)
        , sessionId(session)
        , taskId(id)
    {
    }

    Task(Command cmd, const QString &tar, const QVariantList &arg)
        : Task(cmd, tar, arg, QUuid())
    {
    }

    Task(const QString &cmd, const QString &tar, const QVariantList &arg, const QUuid &session, uint64_t id=0)
        : Task(Task::stringToCommand(cmd), tar, arg, session, id)
    {
    }

    Task()
        : Task(Undefined, QString(), QVariantList())
    {
    }

    Task(const Task &other)
        : command(other.command)
        , commandString(other.commandString)
        , target(other.target)
        , targetObject(other.targetObject)
        , targetName(other.targetName)
        , args(other.args)
        , sessionId(other.sessionId)
        , taskId(other.taskId)
    {
    }

    // ------------- String to Enum Conversion ----- //
    static Command stringToCommand(const QString &cmd)
    {
        static QMap<QString, Command> str2cmd;
        if (str2cmd.isEmpty()) {
            str2cmd["SetFigure"] = SetFigure;
            str2cmd["SetProperty"] = SetProperty;
            str2cmd["GetProperty"] = GetProperty;
            str2cmd["Invoke"] = Invoke;

            str2cmd["SetGui"] = SetGui;
            str2cmd["SetParam"] = SetGuiProperty;

            str2cmd["AddComponent"] = AddComponent;
        }

        return str2cmd.value(cmd, Undefined);
    }

    static QString commandToString(Command cmd)
    {
        static QMap<Command, QString> cmd2str;
        if (cmd2str.isEmpty()) {
            cmd2str[Undefined] = "Undefined";
            cmd2str[SetFigure] = "SetFigure";
            cmd2str[SetProperty] = "SetProperty";
            cmd2str[GetProperty] = "GetProperty";
            cmd2str[Invoke] = "Invoke";

            cmd2str[SetGui] = "SetGui";
            cmd2str[SetGuiProperty] = "SetParam";

            cmd2str[AddComponent] = "AddComponent";
        }
        return cmd2str.value(cmd, "Unknown");
    }

    // ----------------------- Members ------------- //
    Command command;
    QString commandString;
    QString target;
    NutmegObject *targetObject;
    QString targetName;
    QVariantList args;

    QUuid sessionId;
    uint32_t taskId;

};

Q_DECLARE_METATYPE(Task)

#endif // TASK_H
