#ifndef NUTMEGERROR_H
#define NUTMEGERROR_H

#include <QString>
#include <QVariantMap>

#include <exception>

#include "task.h"

// TODO: Consider putting all the docs at the top here...

/*!
 * \brief Defines he basic error type in Nutmeg.
 *
 * NutmegError has three members for providing information about an error: name and message strings
 * and a details map/dictionary. New errors are defined by inheriting from NutmegError and
 * populating these variables as necessary.
 */
class NutmegError : std::exception {
public:
    NutmegError(const Task &task, const QString &name, const QString &message)
        : std::exception()
        , name(name)
        , message(message)
        , details(QVariantMap())
        , task(task)
    {}

    NutmegError()
        : NutmegError(Task(), QString(), QString())
    {}

    NutmegError(const NutmegError &other)
        : std::exception()
        , name(other.name)
        , message(other.message)
        , details(other.details)
        , task(other.task)
    {}

    /*!
     * \brief Unique name for identifying the error.
     */
    QString name;

    /*!
     * \brief Brief message for indicating to the user the cause of the error.
     */
    QString message;

    /*!
     * \brief Any extra details specific to the error.
     */
    QVariantMap details;

    /*!
     * \brief The task attached to this exception
     */
    Task task;
};

/*!
 * \brief Raised when a sent command is unknown to Nutmeg's Controller. The NutmegError#details
 * map contains { "command": str } of the unknown command string.
 */
class UnknownCommand : public NutmegError {
public:
    UnknownCommand(const Task &task)
        : NutmegError(task, "UnknownCommand", QString("Unknown command, %1").arg(task.commandString))
    {
        details["command"] = task.commandString;
    }

    /*!
     * \property UnknownCommand::name
     * \brief "UnknownCommand"
     */
};

/*!
 * \brief Raised when the argument count for a command is wrong. The NutmegError#details map
 * contains the command, and expected and given argument counts ( { "command": str, "expected": int, "given": int } )
 */
class BadArgumentCount : public NutmegError {
public:
    BadArgumentCount(const Task &task, int expected, int given)
        : NutmegError(
              task,
              "BadArgumentCount",
              QString("Command, %1, expects %2 arguments but %3 given.")\
                .arg(task.commandString).arg(expected).arg(given))
    {
        details["command"] = task.commandString;
        details["expected"] = expected;
        details["given"] = given;
    }

    /*!
     * \property BadArgumentCount::name
     * \brief "BadArgumentCount"
     */
};

/*!
 * \brief Raised when the Controller is unable to find the NutmegObject targeted by the Task.
 * The NutmegError#details map contains the invalid target ( { "target": str } )
 */
class InvalidNutmegObject : public NutmegError {
public:
    InvalidNutmegObject(const Task &task, QString msg = QString())
        : NutmegError(task, "InvalidNutmegObject", msg)
    {
        if (message.isEmpty())
            message = QString("Unable to locate object \"%1\".").arg(task.target);

        details["target"] = task.target;
    }

    /*!
     * \property InvalidNutmegObject::name
     * \brief "InvalidNutmegObject"
     */
};

/*!
 * \brief Raised when the Controller is unable to find the property or method targeted by the Task.
 * The NutmegError#details map contains the invalid target ( { "target": str } )
 */
class InvalidTarget : public NutmegError {
public:
    InvalidTarget(const Task &task, QString msg = QString())
        : NutmegError(task, "InvalidTarget", msg)
    {
        if (message.isEmpty())
            message = QString("Unable to locate \"%1\".").arg(task.target);

        details["target"] = task.target;
    }

    /*!
     * \property InvalidTarget::name
     * \brief "InvalidTarget"
     */
};

/*!
 * \brief Caused by errors in Figure's or GUI's QML description. The NutmegError#details map
 * contains "line" and "column" entries to indicate where the error occured in the QML.
 */
class QmlParserError : public NutmegError {
public:
    QmlParserError(const Task &task, const QString &message, int line, int column)
        : NutmegError(task, "QmlParseError", message)
    {
        details["line"] = QVariant::fromValue(line);
        details["column"] = QVariant::fromValue(column);
    }

    /*!
     * \property QmlParseError::name
     * \brief "QmlParserError"
     */
};

class FigureError : public NutmegError {
public:
    FigureError(const Task &task, const QString &message)
        : NutmegError(task, "FigureError", message)
    {
    }

    /*!
     * \property FigureError::name
     * \brief "FigureError"
     */
};

class GuiError : public NutmegError {
public:
    GuiError(const Task &task, const QString &message)
        : NutmegError(task, "GuiError", message)
    {
    }

    /*!
     * \property GuiError::name
     * \brief "GuiError"
     */
};

class PropertyError : public NutmegError {
public:
    PropertyError(const Task &task, const QString &message, const QString &propType, const QString &propName)
        : NutmegError(task, "PropertyError", message)
    {
        details["propertyType"] = propType;
        details["propertyName"] = propName;
    }

    /*!
     * \property PropertyError::name
     * \brief "PropertyError"
     */
};

class InvokeError : public NutmegError {
public:
    InvokeError(const Task &task, const QString &message)
        : NutmegError(task, "InvokeError", message)
    {
    }

    /*!
     * \property MethodError::name
     * \brief "MethodError"
     */
};

Q_DECLARE_METATYPE(NutmegError)

#endif // NUTMEGERROR_H
