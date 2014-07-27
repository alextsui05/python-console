#ifndef ATSUI_CONSOLE_H
#define ATSUI_CONSOLE_H
#include <QTextEdit>

class QWidget;
class QKeyEvent;

namespace atsui {

class Console : public QTextEdit
{
    Q_OBJECT

public:
    Console( QWidget* parent = 0 );

protected:
    // override QTextEdit
    virtual void keyPressEvent( QKeyEvent* e );

    virtual void handleReturnKeyPress( );

    QString getLine( );
    bool cursorIsOnInputLine( );
    bool inputLineIsEmpty( );
    bool canBackspace( );
    void displayPrompt( );

    static const QString PROMPT;
};

}

#endif // ATSUI_CONSOLE_H
