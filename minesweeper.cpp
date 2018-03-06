#include "minesweeper.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

Minesweeper::Minesweeper(QWidget *parent)
    : QWidget(parent)
    , m_timeWidget(new QLCDNumber(this))
    , m_minesMarkedWidget(new QLCDNumber(this))
    , m_gridLayout(new QGridLayout(this))
{
    QHBoxLayout* hboxLayout = new QHBoxLayout(this);
    QVBoxLayout* vboxLayout = new QVBoxLayout(this);
    QPushButton* restartButton = new QPushButton("Restart", this);

    QWidget* clockPanelWidget = new QWidget(this);
    hboxLayout->addWidget(m_timeWidget);
    hboxLayout->addWidget(restartButton);
    hboxLayout->addWidget(m_minesMarkedWidget);
    clockPanelWidget->setMaximumHeight(40);
    clockPanelWidget->setLayout(hboxLayout);
    connect(restartButton, &QPushButton::clicked, this, &Minesweeper::SlotInitialize);

    m_sessionTimer.start();
    m_sessionTimer.setInterval(1000);
    m_sessionTimer.setSingleShot(false);
    connect(&m_sessionTimer, &QTimer::timeout, this, &Minesweeper::SlotIncreaseTimer);

    m_gridLayout->setSpacing(0);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);
    m_gridLayout->setMargin(0);
    SlotInitialize();
    QWidget* gridPanelWidget = new QWidget(this);
    gridPanelWidget->setMinimumSize(m_width * m_height, m_width * m_height);
    gridPanelWidget->resize(m_width * m_height, m_width * m_height);
    gridPanelWidget->setLayout(m_gridLayout);

    vboxLayout->addWidget(clockPanelWidget);
    vboxLayout->addWidget(gridPanelWidget);
    setLayout(vboxLayout);

    setMaximumWidth(m_width * m_height);
    setMaximumHeight(m_width * m_height + 40);
}

void Minesweeper::SlotCellClicked(Cell* cell, QMouseEvent* event)
{
    if (m_lost)
    {
        emit SignalLost();
    }

    if (event->button() == Qt::MouseButton::LeftButton
        && cell->state != Cell::State::FLAG)
    {
        //If a mine, set to mine
        if (cell->mine)
        {
            cell->SlotSetState(Cell::State::MINE);
            m_lost = true;
        }
        //If an empty, open all adjacent empty cells
        else if (cell->number == Cell::Number::ZERO)
        {
            QSet<Cell const*> visitedCells;
            QVector<Cell const*> cellStack{cell};

            while (!cellStack.empty())
            {
                Cell const* currentCell = cellStack.back();
                cellStack.pop_back();

                if (visitedCells.find(currentCell) == visitedCells.end())
                {
                    visitedCells.insert(currentCell);

                    for (auto adjacent : currentCell->adjacent)
                    {
                        if (adjacent->number != Cell::Number::ZERO)
                        {
                            adjacent->SlotSetNumber(adjacent->number);
                            adjacent->SlotSetState(Cell::State::NUMBER);
                        }
                        else
                        {
                            adjacent->SlotSetState(Cell::State::OPEN);
                            cellStack.push_back(adjacent);
                        }
                    }
                }
            }

            cell->SlotSetState(Cell::State::OPEN);
        }
        //If a number, set propper number
        else
        {
            cell->SlotSetNumber(cell->number);
            cell->SlotSetState(Cell::State::NUMBER);
        }
    }
    else if (event->button() == Qt::MouseButton::RightButton
             && (cell->state == Cell::State::CLOSE || cell->state == Cell::State::FLAG))
    {
        if (cell->state == Cell::State::FLAG)
        {
            cell->SlotSetState(Cell::State::CLOSE);
            if (cell->mine)
            {
                m_correctFlags--;
            }
        }
        else
        {
            cell->SlotSetState(Cell::State::FLAG);
            if (cell->mine)
            {
                m_correctFlags++;
            }
        }

        m_minesMarkedWidget->display(m_minesNumber - m_correctFlags);

        if (m_correctFlags == m_minesNumber)
        {
            emit SignalWin();
        }
    }
}

void Minesweeper::SlotInitialize()
{
    m_minesMarkedWidget->display(m_minesNumber);

    //Clear cells and layout
    m_lost = false;
    m_correctFlags = 0;
    m_sessionTime = 0;
    m_timeWidget->display(static_cast<int>(m_sessionTime));
    for (Cell* cell : m_cells)
    {
        cell->deleteLater();
    }
    m_cells.clear();

    connect(this, &Minesweeper::SignalWin, this, &Minesweeper::SlotInitialize);
    connect(this, &Minesweeper::SignalLost, this, &Minesweeper::SlotInitialize);

    //Initialize
    InitializeCells();
    LinkCells();
    PlaceMines();
    CalculateNumbers();
}

void Minesweeper::SlotIncreaseTimer()
{
    if (!m_lost)
    {
        m_sessionTime++;
        m_timeWidget->display(static_cast<int>(m_sessionTime));
    }
}

void Minesweeper::InitializeCells()
{
    m_cells.reserve(m_width * m_height);
    for (quint8 i = 0; i < m_height; ++i)
    {
        for (quint8 j = 0; j < m_width; ++j)
        {
            m_cells.push_back(new Cell(this));
            m_gridLayout->addWidget(m_cells.back(), i, j);
            connect(m_cells.back(), &Cell::SignalClicked, this, &Minesweeper::SlotCellClicked);
        }
    }
}

void Minesweeper::LinkCells()
{
    for (qint8 i = 0; i < m_height; ++i)
    {
        for (qint8 j = 0; j < m_width; ++j)
        {
            bool const hasPreviousCell = (j - 1 >= 0);
            bool const hasNextCell = (j + 1 < m_width);
            auto& adjacent = m_cells[j + i * m_height]->adjacent;

            //Filling upper cell's pointers
            if (i - 1 >= 0)
            {
                adjacent.push_back(hasPreviousCell ? (m_cells[j - 1 + (i - 1) * m_height]) : nullptr);
                adjacent.push_back(hasNextCell     ? (m_cells[j + 1 + (i - 1) * m_height]) : nullptr);
                adjacent.push_back(m_cells[j + (i - 1) * m_height]);
            }

            adjacent.push_back(hasPreviousCell ? m_cells[j - 1 + i * m_height] : nullptr);
            adjacent.push_back(hasNextCell     ? m_cells[j + 1 + i * m_height] : nullptr);

            //Filling bottom cell's pointers
            if (i + 1 < m_height)
            {
                adjacent.push_back(hasPreviousCell ? (m_cells[j - 1 + (i + 1) * m_height]) : nullptr);
                adjacent.push_back(hasNextCell     ? (m_cells[j + 1 + (i + 1) * m_height]) : nullptr);
                adjacent.push_back(m_cells[j + (i + 1) * m_height]);
            }

            adjacent.erase(std::remove(adjacent.begin(), adjacent.end(), nullptr), adjacent.end());
        }
    }
}

void Minesweeper::PlaceMines()
{
    std::knuth_b randomDevice;
    std::default_random_engine eng(randomDevice());
    std::uniform_int_distribution<int> uniform_dist(0, m_cells.size());

    quint8 minePlaceCounter = 0;
    while (minePlaceCounter < m_minesNumber)
    {
        quint8 const index = uniform_dist(eng);
        if (!m_cells[index]->mine)
        {
            m_cells[index]->mine = true;
            minePlaceCounter++;
        }
    }
}

void Minesweeper::CalculateNumbers()
{
    for(auto cell : m_cells)
    {
        quint8 mines = 0;
        for (auto adjacent : cell->adjacent)
        {
            if (adjacent->mine)
            {
                ++mines;
            }
        }
        cell->SlotSetNumber(static_cast<Cell::Number>(mines));
        cell->SlotSetState(Cell::State::CLOSE);
    }
}
