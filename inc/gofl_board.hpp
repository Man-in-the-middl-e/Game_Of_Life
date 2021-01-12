#ifndef GOFL_BOARD_HPP
#define GOFL_BOARD_HPP
#include<memory>
#include<iostream>



enum CELL_STATE : char{
    LIVE = '#',
    DEAD = ' '
};

/*
    2D Row-major array
    You cannot copy object of this class because  uniqu_ptr is used to store data
*/
class GoflBoard
{
public:
    GoflBoard(uint32_t n_rows, uint32_t n_cols);
    GoflBoard(uint32_t n_rows, uint32_t n_cols, char default_val);
    //disallow implicit conversion when init with default value
    template<class T>
    GoflBoard(uint32_t n_rows, uint32_t n_cols, T default_val) = delete;
    GoflBoard(std::initializer_list<std::initializer_list<char>> ls);
    GoflBoard(const std::string file_name);
   

    uint32_t Rows() const;
    uint32_t Cols() const;

    char *operator[](uint32_t row);
    const char *operator[](uint32_t row) const;

    char &operator()(uint32_t row, uint32_t col);
    const char &operator()(uint32_t row, uint32_t col) const;
     
    void Run();
    void Print(std::ostream& stream = std::cout) const;
    //don't forget to initialize size_ and capacity_ inside ctors
    bool Empty() const;

private:
    uint32_t rows_;
    uint32_t cols_;
    uint32_t size_;
    uint32_t capacity_;
    std::unique_ptr<char[]> data;

    static const  uint32_t kScreenRefreshRate = 100000; //mcrs
    
    void NextBoardState();
    void SaveToFile() const;
    void PrintLine(std::ostream& strea = std::cout) const;
    void RefreshScreen(const char* buffer, size_t buffer_size);
    CELL_STATE NextCellState(CELL_STATE cell, uint16_t n_neigh) const ;
    uint16_t NumberOfLiveNeighs(uint32_t row, uint32_t col)  const ; 
};
#endif