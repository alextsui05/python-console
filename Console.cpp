#include "Console.h"
#include "Interpreter.h"

#include <iostream>
#include <QKeyEvent>

const QString Console::PROMPT = ">>> ";
const QColor Console::NORMAL_COLOR = QColor::fromRgbF( 0, 0, 0 );
const QColor Console::ERROR_COLOR = QColor::fromRgbF( 1.0, 0, 0 );
const QColor Console::OUTPUT_COLOR = QColor::fromRgbF( 0, 0, 1.0 );

Console::Console( QWidget* parent ):
    QTextEdit( parent ),
    m_interpreter( new Interpreter )
{
    displayPrompt( );
}

Console::~Console( )
{
    delete m_interpreter;
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
        std::cout << line.toStdString( ) << "\n";

        // TODO: Feed line to the python interpreter
        int errorCode;
        std::string res = m_interpreter->interpret( line.toStdString( ), &errorCode );
        if ( errorCode )
        {
            setTextColor( ERROR_COLOR );
        }
        else
        {
            setTextColor( OUTPUT_COLOR );
        }

        std::cout << res << "\n";
        if ( res.size( ) )
        {
            if ( ! errorCode )
            append("");
            append(res.c_str());
        }

        setTextColor( NORMAL_COLOR );

        // set up the next line on the console
        append("");
        displayPrompt( );
    }
    else
    {
        m_interpreter->test( );
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
