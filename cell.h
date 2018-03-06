#ifndef CELL_H
#define CELL_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPixmap>
#include <QMouseEvent>

class Cell : public QWidget
{
    Q_OBJECT
public:
    enum class State  : quint8;
    enum class Number : quint8;

    explicit Cell(QWidget *parent = nullptr);

    Number number = Number::ZERO;
    State state = State::CLOSE;
    QVector<Cell*> adjacent;
    bool mine = false;

    enum class State  : quint8
    {
        OPEN,
        CLOSE,
        NUMBER,
        FLAG,
        MINE,
    };

    enum class Number : quint8
    {
        ZERO     = 0,
        ONE      = 1,
        TWO      = 2,
        THREE    = 3,
        FOUR     = 4,
        FIVE     = 5,
        SIX      = 6,
        SEVEN    = 7,
        EIGHT    = 8,
    };

signals:
    void SignalClicked(Cell* cell, QMouseEvent* event);

public slots:
    void SlotSetState(State state);
    void SlotSetNumber(Number number);

protected:
    void mousePressEvent(QMouseEvent* event);

private:
    QPixmap m_pixmap;
    QLabel* m_label;
    QVBoxLayout* m_layout;
};

#endif // CELL_H
