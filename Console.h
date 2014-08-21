/**
python-console
Copyright (C) 2014  Alex Tsui

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef ATSUI_CONSOLE_H
#define ATSUI_CONSOLE_H
#include <QTextEdit>
#include <QColor>
#include "ParseHelper.h"
#include "ParseListener.h"

class QWidget;
class QKeyEvent;
class Interpreter;

class Console : public QTextEdit, ParseListener
{
    Q_OBJECT

public:
    Console( QWidget* parent = 0 );
    virtual ~Console( );

protected:
    // override QTextEdit
    virtual void keyPressEvent( QKeyEvent* e );

    virtual void handleReturnKeyPress( );

    /**
    Handle a compilable chunk of Python user input.
    */
    virtual void parseEvent( const ParseMessage& message );

    QString getLine( );
    bool cursorIsOnInputLine( );
    bool inputLineIsEmpty( );
    bool canBackspace( );
    void displayPrompt( );
    void autocomplete( );

    static const QString PROMPT;
    static const QString MULTILINE_PROMPT;

    static const QColor NORMAL_COLOR;
    static const QColor ERROR_COLOR;
    static const QColor OUTPUT_COLOR;

    Interpreter* m_interpreter;
    ParseHelper m_parseHelper;
};

#endif // ATSUI_CONSOLE_H
