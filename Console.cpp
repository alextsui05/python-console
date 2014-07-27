#include "Console.h"

#include <iostream>
#include <QKeyEvent>

namespace atsui {

const QString Console::PROMPT = ">>> ";

Console::Console( QWidget* parent ):
    QTextEdit( parent )
{
    displayPrompt( );
}

void Console::keyPressEvent( QKeyEvent* e )
{
    switch ( e->key() )
    {
        case Qt::Key_Return:
            handleReturnKeyPress( );
            return;

        case Qt::Key_Backspace:
            if ( ! canBackspace( ) )
                return;
    }

    QTextEdit::keyPressEvent( e );
}

void Console::handleReturnKeyPress( )
{
    if ( ! cursorIsOnInputLine( ) )
    {
        return;
    }

    QString line = getLine( );
    if ( line != "" )
    {
        // TODO: Feed line to the python interpreter

        // TODO: Append the interpreter output to console
        std::cout << line.toStdString( ) << "\n";

        // set up the next line on the console
        append("");
        displayPrompt( );
    }
}

QString Console::getLine( )
{
    QTextCursor cursor = textCursor();
    cursor.movePosition( QTextCursor::StartOfLine );
    cursor.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor, Console::PROMPT.size( ) );
    cursor.movePosition( QTextCursor::EndOfLine, QTextCursor::KeepAnchor );
    QString line = cursor.selectedText( );
    cursor.clearSelection( );
    return line;
}

bool Console::cursorIsOnInputLine( )
{
    int cursorBlock = textCursor( ).blockNumber( );
    QTextCursor bottomCursor = textCursor( );
    bottomCursor.movePosition( QTextCursor::End );
    int bottomBlock = bottomCursor.blockNumber( );
    return ( cursorBlock == bottomBlock );
}

bool Console::inputLineIsEmpty( )
{
    QTextCursor bottomCursor = textCursor( );
    bottomCursor.movePosition( QTextCursor::End );
    int col = bottomCursor.columnNumber( );
    return ( col == Console::PROMPT.size( ) );
}

bool Console::canBackspace( )
{
    if ( ! cursorIsOnInputLine( ) )
    {
        return false;
    }

    if ( inputLineIsEmpty( ) )
    {
        return false;
    }

    return true;
}

void Console::displayPrompt( )
{
    QTextCursor cursor = textCursor();
    cursor.insertText( Console::PROMPT );
    cursor.movePosition( QTextCursor::EndOfLine );
}

} // namespace atsui
