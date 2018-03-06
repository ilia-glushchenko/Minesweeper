#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <cell.h>
#include <QWidget>
#include <QVector>
#include <QLCDNumber>
#include <QGridLayout>
#include <QTimer>

class Minesweeper : public QWidget
{
    Q_OBJECT
public:
    explicit Minesweeper(QWidget *parent = nullptr);

signals:
    void SignalLost();
    void SignalWin();

public slots:
    void SlotCellClicked(Cell* cell, QMouseEvent* event);
    void SlotInitialize();
    void SlotIncreaseTimer();

private:
    static constexpr quint8 m_width = 16;
    static constexpr quint8 m_height = 16;
    static constexpr quint8 m_minesNumber = 40;

    QTimer m_sessionTimer;
    QLCDNumber* m_timeWidget;
    QLCDNumber* m_minesMarkedWidget;
    QGridLayout* m_gridLayout;
    QVector<Cell*> m_cells;
    quint8 m_correctFlags = 0;
    quint64 m_sessionTime = 0;
    bool m_lost = false;

    void InitializeCells();
    void LinkCells();
    void PlaceMines();
    void CalculateNumbers();
};

#endif // MINESWEEPER_H
