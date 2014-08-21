#include "Console.h"
#include "Interpreter.h"
#include "ColumnFormatter.h"

#include <iostream>
#include <QKeyEvent>
#include <QFont>

const QString Console::PROMPT = ">>> ";
const QString Console::MULTILINE_PROMPT = "... ";
const QColor Console::NORMAL_COLOR = QColor::fromRgbF( 0, 0, 0 );
const QColor Console::ERROR_COLOR = QColor::fromRgbF( 1.0, 0, 0 );
const QColor Console::OUTPUT_COLOR = QColor::fromRgbF( 0, 0, 1.0 );

Console::Console( QWidget* parent ):
    QTextEdit( parent ),
    m_interpreter( new Interpreter )
{
    QFont font;
    font.setFamily("Courier New");
    setFont(font);
    m_parseHelper.subscribe( this );
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

        case Qt::Key_Tab:
            autocomplete( );
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

#ifndef NDEBUG
    std::cout << line.toStdString( ) << "\n";
#endif

    m_parseHelper.process( line.toStdString( ) );
    if ( m_parseHelper.buffered( ) )
    {
        append("");
        displayPrompt( );
    }
}

void Console::parseEvent( const ParseMessage& message )
{
    // handle invalid user input
    if ( message.errorCode )
    {
        setTextColor( ERROR_COLOR );
        append(message.message.c_str());

        setTextColor( NORMAL_COLOR );
        append("");
        displayPrompt( );
        return;
    }

    // interpret valid user input
    int errorCode;
    std::string res;
    if ( message.message.size() )
        res = m_interpreter->interpret( message.message, &errorCode );
    if ( errorCode )
    {
        setTextColor( ERROR_COLOR );
    }
    else
    {
        setTextColor( OUTPUT_COLOR );
    }

    if ( res.size( ) )
    {
        append(res.c_str());
    }

    setTextColor( NORMAL_COLOR );

    // set up the next line on the console
    append("");
    displayPrompt( );
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
    if ( m_parseHelper.buffered( ) )
    {
        cursor.insertText( Console::MULTILINE_PROMPT );
    }
    else
    {
        cursor.insertText( Console::PROMPT );
    }
    cursor.movePosition( QTextCursor::EndOfLine );
}

void Console::autocomplete( )
{
    QString line = getLine( );
    const std::list<std::string>& suggestions =
        m_interpreter->suggest( line.toStdString( ) );
    ColumnFormatter fmt;
    fmt.setItems(suggestions.begin(), suggestions.end());
    fmt.format(width() / 10);
    setTextColor( OUTPUT_COLOR );
    const std::list<std::string>& formatted = fmt.formattedOutput();
    for (std::list<std::string>::const_iterator it = formatted.begin();
        it != formatted.end(); ++it)
    {
        append(it->c_str());
    }
    std::cout << width() << "\n";
    setTextColor( NORMAL_COLOR );
    // set up the next line on the console
    append("");
    displayPrompt( );
}
