#include "gofl_board.hpp"

#include <fstream>
#include <vector>
#include <sstream>
//public members definitions

#ifdef __unix__
#include <unistd.h>
void GoflBoard::RefreshScreen(const char *buffer, size_t buffer_size)
{
    write(STDOUT_FILENO, "\x1b[2J", 4);   //clear screen
    write(STDOUT_FILENO, "\x1b[1;1H", 7); //set cursor to the top left corner
    write(STDOUT_FILENO, buffer, buffer_size);
}
#endif

GoflBoard::GoflBoard(uint32_t n_rows, uint32_t n_cols) : rows_{n_rows},
                                                         cols_{n_cols},
                                                         data{std::make_unique<char[]>(rows_ * cols_)}
{
}

GoflBoard::GoflBoard(uint32_t n_rows, uint32_t n_cols, char default_val) : GoflBoard(n_rows, n_cols)
{
    for (uint32_t i = 0; i < rows_ * cols_; ++i)
    {
        data[i] = default_val;
    }
}
GoflBoard::GoflBoard(std::initializer_list<std::initializer_list<char>> ls) : GoflBoard(ls.size(), ls.begin()->size())
{
    uint32_t i = 0;
    for (const auto &row : ls)
    {
        if (row.size() != cols_)
        {
            throw std::out_of_range("Number of colums must be the same  on each row");
        }
        for (const auto col : row)
        {
            data[i++] = col;
        }
    }
}

GoflBoard::GoflBoard(const std::string file_name)
{
    //0-  DEAD
    //1 - LIVE
    //format: 000101..., each row in new line

    std::fstream in(file_name, std::ios::in);
    std::vector<std::string> cells;
    if (in.is_open())
    {
        //read file
        for (std::string line; std::getline(in, line);)
        {
            cells.push_back(line);
        }
        //consturct board
        this->rows_ = cells.size();
        this->cols_ = cells.begin()->size();
        this->data = std::make_unique<char[]>(rows_ * cols_);

        //fill board
        for (uint32_t i = 0; i < cells.size(); ++i)
        {
            for (uint32_t j = 0; j < cells[i].size(); ++j)
            {
                this->operator()(i, j) = (cells[i][j] == '1' ? LIVE : DEAD);
            }
        }
    }
    else
    {
        std::cout << "Could not open file " << file_name << std::endl;
    }
}

uint32_t GoflBoard::Rows() const
{
    return rows_;
}

uint32_t GoflBoard::Cols() const
{
    return cols_;
}

char *GoflBoard::operator[](uint32_t row)
{
    return row * cols_ + data.get();
}

const char *GoflBoard::operator[](uint32_t row) const
{
    return row * cols_ + data.get();
}

char &GoflBoard::operator()(uint32_t row, uint32_t col)
{
    return data[row * cols_ + col];
}

const char &GoflBoard::operator()(uint32_t row, uint32_t col) const
{
    return data[row * cols_ + col];
}

void GoflBoard::Run()
{
    while (true)
    {
        std::stringstream stream;
        this->Print(stream);
        this->NextBoardState();
        RefreshScreen(stream.str().c_str(), stream.str().size());
#ifdef __unix__
        usleep(kScreenRefreshRate);
#endif
    }
}

void GoflBoard::Print(std::ostream &stream) const
{

    PrintLine(stream);
    for (uint32_t row = 0; row < rows_; ++row)
    {
        for (uint32_t col = 0; col < cols_; ++col)
        {
            char cell = this->operator()(row, col);
            stream << "| " << cell << " ";
        }
        stream << "|" << std::endl;
        PrintLine(stream);
    }
}

bool GoflBoard::Empty() const
{
    return data.get() == nullptr;
}

//private members definitions

void GoflBoard::PrintLine(std::ostream &stream) const
{
    for (uint32_t col = 0; col < cols_ * 4 + 1; ++col)
        stream << "-";
    stream << std::endl;
}

CELL_STATE GoflBoard::NextCellState(CELL_STATE cell, uint16_t n_neigh) const
{
    switch (cell)
    {
    case CELL_STATE::LIVE:
    {
        if (n_neigh <= 1 || n_neigh > 3)
            return CELL_STATE::DEAD;
        else if (n_neigh == 2 || n_neigh == 3)
            return CELL_STATE::LIVE;
    }
    case CELL_STATE::DEAD:
        if (n_neigh == 3)
            return CELL_STATE::LIVE;
    }
    return cell;
}

uint16_t GoflBoard::NumberOfLiveNeighs(uint32_t row, uint32_t col) const
{
    uint16_t n_neigh = 0;
    //make board circular
    //if board size >= 3x3 each cell will have 8 neighbours
    for (int x = row + 1, x_neig = 3; x_neig > 0; x = (x - 1 < 0) ? rows_ - 1 : x - 1, --x_neig)
    {
        for (int y = col + 1, y_neig = 3; y_neig > 0; y = (y - 1 < 0) ? cols_ - 1 : y - 1, --y_neig)
        {
            if (x != row || y != col)
            {
                n_neigh += ((this->operator()(x, y) == CELL_STATE::LIVE) ? 1 : 0);
            }
        }
    }
    return n_neigh;
}

void GoflBoard::NextBoardState()
{
    
    GoflBoard next_board_state(rows_, cols_, (char)CELL_STATE::DEAD);
    for (uint32_t row = 0; row < rows_; ++row)
    {
        for (uint32_t col = 0; col < cols_; ++col)
        {
            CELL_STATE current_cell = static_cast<CELL_STATE>(this->operator()(row, col));
            next_board_state[row][col] = NextCellState(current_cell, NumberOfLiveNeighs(row, col));
        }
    }
    for (uint32_t row = 0; row < rows_; ++row)
    {
        for (uint32_t col = 0; col < cols_; ++col)
        {
            this->operator()(row, col) = next_board_state(row, col);
        }
    }
}
