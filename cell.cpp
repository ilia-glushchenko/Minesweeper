#include "cell.h"

Cell::Cell(QWidget *parent)
    : QWidget(parent)
    , m_label(new QLabel(this))
    , m_layout(new QVBoxLayout(this))

{
    m_label->setMargin(0);
    m_layout->addWidget(m_label);
    m_layout->setSpacing(0);
    m_layout->setMargin(0);

    setLayout(m_layout);
    resize(16, 16);
}

void Cell::SlotSetState(Cell::State s)
{
    state = s;

    switch (state)
    {
        case State::NUMBER:
            SlotSetNumber(number);
            break;
        case State::OPEN:
            m_pixmap.load("0.png");
            break;
        case State::CLOSE:
            m_pixmap.load("unopened.png");
            break;
        case State::FLAG:
            m_pixmap.load("flag.png");
            break;
        case State::MINE:
            m_pixmap.load("mine.png");
            break;
        default:
            break;
    }

    m_label->setPixmap(m_pixmap);
}

void Cell::SlotSetNumber(Cell::Number n)
{
    number = n;

    switch (number)
    {
        case Number::ZERO:
            m_pixmap.load("0.png");
            break;
        case Number::ONE:
            m_pixmap.load("1.png");
            break;
        case Number::TWO:
            m_pixmap.load("2.png");
            break;
        case Number::THREE:
            m_pixmap.load("3.png");
            break;
        case Number::FOUR:
            m_pixmap.load("4.png");
            break;
        case Number::FIVE:
            m_pixmap.load("5.png");
            break;
        case Number::SIX:
            m_pixmap.load("6.png");
            break;
        case Number::SEVEN:
            m_pixmap.load("7.png");
            break;
        case Number::EIGHT:
            m_pixmap.load("8.png");
            break;
        default:
            break;
    }

    m_label->setPixmap(m_pixmap);
}

void Cell::mousePressEvent(QMouseEvent* event)
{
    emit SignalClicked(this, event);
}
