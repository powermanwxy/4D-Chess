#include "ChessBoard.h"
#include <iostream>
#include <omp.h>
#include <stack>
using namespace std;

double predictNum;
double calNum;
stack<PicesPos> history;

ChessBoard::ChessBoard(QObject *parent) : QObject(parent)
{

}

void ChessBoard::init()
{
    for(int z = 0;z < 4;z++)
    {
        for(int y = 0;y < 4;y++)
        {
            for(int x = 0;x < 4;x++)
            {
                chessBoard[x][y][z] = chessPicesStatus::empty;
            }
        }
    }

    while (!history.empty()) {
        history.pop();
    }
}

bool ChessBoard::insertPices(int x, int y, int type)
{
    if(x < 0 || x >3 || y < 0 || y > 3)
        return false;

    //check 0-3th floor
    for(int z = 0;z < 4;z++)
    {
        if(chessBoard[x][y][z] == chessPicesStatus::empty)
        {
            chessBoard[x][y][z] = chessPicesStatus(type);
            PicesPos pos;
            pos.x = x;
            pos.y = y;
            pos.z = z;
            pos.type = chessPicesStatus(type);
            history.push(pos);
            return true;
        }
    }

    return false;
}

void ChessBoard::printBoard()
{
    for(int z = 0;z < 4;z++)
    {
        //cout<<"The "<<z<<"th floor"<<endl;
        for(int y = 0;y < 4;y++)
        {
            for(int x = 0;x < 4;x++)
            {
                //cout<<chessBoard[x][y][z]<<" ";
            }
            //cout<<endl;
        }
        //cout<<"\\\\\\\\\\\\\\"<<endl;
    }

}

PicesPosList ChessBoard::getAvailablePos(int board[4][4][4], chessPicesStatus side)
{
    PicesPosList availablePos;

    for(int y = 0;y < 4;y++)
    {
        for(int x = 0;x < 4;x++)
        {
            for(int z = 0; z < 4;z++)
            {
                if(board[x][y][z] == chessPicesStatus::empty)
                {
                    PicesPos pos;
                    pos.x = x;
                    pos.y = y;
                    pos.z = z;
                    pos.type = chessPicesStatus::empty;
                    pos.value = getPosValue(board,&pos,side);
                    availablePos.push_back(pos);
                    break;
                }
            }
        }
    }

    //cout<<"Available Pos:"<<endl;

    for(auto iter = availablePos.begin();iter != availablePos.end(); iter++)
    {
            //cout<<iter->x <<" "<< iter->y <<" " << iter->z << endl;

    }
    //cout<<"Available Size:"<<availablePos.size()<<endl;

    return availablePos;
}

bool ifSimpleFlat(PicesPos pos)
{
    if(pos.x == 1 && pos.y == 0 ||
       pos.x == 2 && pos.y == 0 ||
       pos.x == 0 && pos.y == 1 ||
       pos.x == 3 && pos.y == 1 ||
       pos.x == 0 && pos.y == 2 ||
       pos.x == 3 && pos.y == 2 ||
       pos.x == 1 && pos.y == 3 ||
       pos.x == 2 && pos.y == 3)
        return true;
    else
        return false;
}

int ChessBoard::getPosValue(int board[4][4][4],PicesPos *pos, chessPicesStatus side)
{
    PicesFlatDataList flatList;

   // //cout<<"x:"<<pos->x<<" y:"<<pos->y<<" z:"<<pos->z<<endl;
    /*
    [0]   -
    [1]   |
    [2]   /-
    [3]   -\
    [4]   /_
    [5]   _\
    */
    FlatsData temp0;
    temp0.a = board[0][pos->y][pos->z];
    temp0.b = board[1][pos->y][pos->z];
    temp0.c = board[2][pos->y][pos->z];
    temp0.d = board[3][pos->y][pos->z];
    flatList.push_back(temp0);

    FlatsData temp1;
    temp1.a = board[pos->x][0][pos->z];
    temp1.b = board[pos->x][1][pos->z];
    temp1.c = board[pos->x][2][pos->z];
    temp1.d = board[pos->x][3][pos->z];
    flatList.push_back(temp1);

    FlatsData temp2;
    temp2.a = board[0][pos->y][0];
    temp2.b = board[1][pos->y][1];
    temp2.c = board[2][pos->y][2];
    temp2.d = board[3][pos->y][3];
    flatList.push_back(temp2);

    FlatsData temp3;
    temp3.a = board[0][pos->y][3];
    temp3.b = board[1][pos->y][2];
    temp3.c = board[2][pos->y][1];
    temp3.d = board[3][pos->y][0];
    flatList.push_back(temp3);

    FlatsData temp4;
    temp4.a = board[pos->x][0][0];
    temp4.b = board[pos->x][1][1];
    temp4.c = board[pos->x][2][2];
    temp4.d = board[pos->x][3][3];
    flatList.push_back(temp4);

    FlatsData temp5;
    temp5.a = board[pos->x][0][3];
    temp5.b = board[pos->x][1][2];
    temp5.c = board[pos->x][2][1];
    temp5.d = board[pos->x][3][0];
    flatList.push_back(temp5);

    FlatsData temp6;
    temp6.a = board[pos->x][pos->y][0];
    temp6.b = board[pos->x][pos->y][1];
    temp6.c = board[pos->x][pos->y][2];
    temp6.d = board[pos->x][pos->y][3];
    flatList.push_back(temp6);

    if(!ifSimpleFlat(*pos))
    {
        if(pos->x == 0 && pos->y == 0 ||
           pos->x == 1 && pos->y == 1 ||
           pos->x == 2 && pos->y == 2 ||
           pos->x == 3 && pos->y == 3)
        {
            FlatsData temp7;
            temp7.a = board[0][0][0];
            temp7.b = board[1][1][1];
            temp7.c = board[2][2][2];
            temp7.d = board[3][3][3];
            flatList.push_back(temp7);

            FlatsData temp8;
            temp8.a = board[0][0][3];
            temp8.b = board[1][1][2];
            temp8.c = board[2][2][1];
            temp8.d = board[3][3][0];
            flatList.push_back(temp8);
        }
        else
        {
            FlatsData temp7;
            temp7.a = board[3][0][0];
            temp7.b = board[2][1][1];
            temp7.c = board[1][2][2];
            temp7.d = board[0][3][3];
            flatList.push_back(temp7);

            FlatsData temp8;
            temp8.a = board[3][0][3];
            temp8.b = board[2][1][2];
            temp8.c = board[1][2][1];
            temp8.d = board[0][3][0];
            flatList.push_back(temp8);
        }

    }

    int val = getFlatPicesValue(flatList,side);

    return val;
}

int ChessBoard::getFlatPicesValue(PicesFlatDataList flats, chessPicesStatus status)
{
    int value = 0,whiteNum = 0,blackNum = 0;

    for(auto iter = flats.begin();iter != flats.end();iter++)
    {
        whiteNum = 0;
        blackNum = 0;

        if(iter->a == chessPicesStatus::black)
            blackNum++;
        else if(iter->a == chessPicesStatus::white)
            whiteNum++;

        if(iter->b == chessPicesStatus::black)
            blackNum++;
        else if(iter->b == chessPicesStatus::white)
            whiteNum++;

        if(iter->c == chessPicesStatus::black)
            blackNum++;
        else if(iter->c == chessPicesStatus::white)
            whiteNum++;

        if(iter->d == chessPicesStatus::black)
            blackNum++;
        else if(iter->d == chessPicesStatus::white)
            whiteNum++;

        if(status == chessPicesStatus::white)
        {
            //Calculating White Picess

            if(blackNum != 0)
                continue;

            if(whiteNum == 0)
                value += 1;
            else if(whiteNum == 1)
                value += 5;
            else if(whiteNum == 2)
                value += 100;
            else if(whiteNum == 3)
                value += 5000;
            else if(whiteNum == 4)
            {
                value += 100000;
            }

        }
        else
        {
            //Calculating Black Picess

            if(whiteNum != 0)
                continue;

            if(blackNum == 0)
                value += 1;
            else if(blackNum == 1)
                value += 5;
            else if(blackNum == 2)
                value += 100;
            else if(blackNum == 3)
                value += 5000;
            else if(blackNum == 4)
            {
                value += 100000;
            }

        }

        //cout<<iter->a<<" "<<iter->b<<" "<<iter->c<<" "<<iter->d<<" "<<value<<endl;
    }
    return value;
}

int ChessBoard::getSideScore(int board[4][4][4], chessPicesStatus side)
{
    PicesFlatDataList flats;

    for(int y = 0;y < 4;y++)
    {
        for(int z = 0;z < 4;z++)
        {
            FlatsData data;
            data.a = board[0][y][z];
            data.b = board[1][y][z];
            data.c = board[2][y][z];
            data.d = board[3][y][z];
            flats.push_back(data);
        }
    }

    for(int x = 0;x < 4;x++)
    {
        for(int z = 0;z < 4;z++)
        {
            FlatsData data;
            data.a = board[x][0][z];
            data.b = board[x][1][z];
            data.c = board[x][2][z];
            data.d = board[x][3][z];
            flats.push_back(data);
        }
    }

    for(int x = 0;x < 4;x++)
    {
        for(int y = 0;y < 4;y++)
        {
            FlatsData data;
            data.a = board[x][y][0];
            data.b = board[x][y][1];
            data.c = board[x][y][2];
            data.d = board[x][y][3];
            flats.push_back(data);
        }
    }

    for(int y = 0;y < 4;y++)
    {
        FlatsData data;
        data.a = board[0][y][0];
        data.b = board[1][y][1];
        data.c = board[2][y][2];
        data.d = board[3][y][3];
        flats.push_back(data);
    }

    for(int x = 0;x < 4;x++)
    {
        FlatsData data;
        data.a = board[x][0][0];
        data.b = board[x][1][1];
        data.c = board[x][2][2];
        data.d = board[x][3][3];
        flats.push_back(data);
    }

    for(int y = 0;y < 4;y++)
    {
        FlatsData data;
        data.a = board[0][y][3];
        data.b = board[1][y][2];
        data.c = board[2][y][1];
        data.d = board[3][y][0];
        flats.push_back(data);
    }

    for(int x = 0;x < 4;x++)
    {
        FlatsData data;
        data.a = board[x][0][3];
        data.b = board[x][1][2];
        data.c = board[x][2][1];
        data.d = board[x][3][0];
        flats.push_back(data);
    }

    for(int z = 0;z < 4;z++)
    {
        FlatsData data;
        data.a = board[0][0][z];
        data.b = board[1][1][z];
        data.c = board[2][2][z];
        data.d = board[3][3][z];
        flats.push_back(data);
    }

    for(int z = 0;z < 4;z++)
    {
        FlatsData data;
        data.a = board[3][0][z];
        data.b = board[2][1][z];
        data.c = board[1][2][z];
        data.d = board[0][3][z];
        flats.push_back(data);
    }

    FlatsData data;
    data.a = board[0][0][0];
    data.b = board[1][1][1];
    data.c = board[2][2][2];
    data.d = board[3][3][3];
    flats.push_back(data);

    data.a = board[0][0][3];
    data.b = board[1][1][2];
    data.c = board[2][2][1];
    data.d = board[3][3][0];
    flats.push_back(data);

    data.a = board[3][0][0];
    data.b = board[2][1][1];
    data.c = board[1][2][2];
    data.d = board[0][3][3];
    flats.push_back(data);

    data.a = board[3][0][3];
    data.b = board[2][1][2];
    data.c = board[1][2][1];
    data.d = board[0][3][0];
    flats.push_back(data);

    int x = getFlatPicesValue(flats, side);

    //cout<<"size:"<<flats.size()<<endl;
    return x;
}

bool comp(const PicesPos &A, const PicesPos &B)
{
    return A.value > B.value;
}

int ChessBoard::findWhiteScoreMaxValue(int board[4][4][4], int deep,int alpha,int beta)
{
    if(deep <= 0)
    {
        int score = getSideScore(board, chessPicesStatus::white) -
                        getSideScore(board, chessPicesStatus::black);
        return score;
    }
    else
    {
        int maxVal = -1000000;
        PicesPosList list = getAvailablePos(board,chessPicesStatus::white);
        sort(list.begin(),list.end(),comp);

        for(auto iter = list.begin();iter != list.end();iter++)
        {
            calNum++;
            //cout<<"Processing:"<<calNum<<" Finished:"<<calNum / predictNum * 100<<"%"<<endl;

            board[iter->x][iter->y][iter->z] = chessPicesStatus::white;
            int val = findWhiteScoreMinValue(board, deep -1,alpha,maxVal > beta ? maxVal : beta);
            if(val > maxVal)
            {
                maxVal = val;
                whiteTargetPos.x = iter->x;
                whiteTargetPos.y = iter->y;
                whiteTargetPos.z = iter->z;
            }
            board[iter->x][iter->y][iter->z] = chessPicesStatus::empty;

            if(maxVal > alpha)
                break;
        }

        return maxVal;
    }
}

int ChessBoard::findWhiteScoreMinValue(int board[4][4][4], int deep,int alpha,int beta)
{

    if(deep <= 0)

    {
        int score = getSideScore(board, chessPicesStatus::white) -
                        getSideScore(board, chessPicesStatus::black);
        return score;
    }
    else
    {
        int minVal = 1000000;
        PicesPosList list = getAvailablePos(board,chessPicesStatus::black);
        sort(list.begin(),list.end(),comp);

        for(auto iter = list.begin();iter != list.end();iter++)
        {
            calNum++;
            //cout<<"Processing:"<<calNum<<" Finished:"<<calNum / predictNum * 100<<"%"<<endl;

            board[iter->x][iter->y][iter->z] = chessPicesStatus::black;
            int val = findWhiteScoreMaxValue(board, deep -1, minVal < alpha ? minVal : alpha ,beta);
            if(val < minVal)
            {
                minVal = val;
                blackTargetPos.x = iter->x;
                blackTargetPos.y = iter->y;
                blackTargetPos.z = iter->z;
            }
            board[iter->x][iter->y][iter->z] = chessPicesStatus::empty;

            if(val < beta)
                break;
        }

        return minVal;
    }
}

int ChessBoard::dfs(int board[4][4][4], int deep)
{
    predictNum = pow(16,deep) / 5 / 5;
    calNum = 0;

    findWhiteScoreMaxValue(board, deep, 1000000,-1000000);

    int whiteScore,blackScore,attackScore,defenceScore;

    board[blackTargetPos.x][blackTargetPos.y][blackTargetPos.z] = chessPicesStatus::white;
    whiteScore = getSideScore(board,chessPicesStatus::white);
    blackScore = getSideScore(board,chessPicesStatus::black);
    defenceScore = whiteScore - blackScore;
    board[blackTargetPos.x][blackTargetPos.y][blackTargetPos.z] = chessPicesStatus::empty;

    board[whiteTargetPos.x][whiteTargetPos.y][whiteTargetPos.z] = chessPicesStatus::white;
    whiteScore = getSideScore(board,chessPicesStatus::white);
    blackScore = getSideScore(board,chessPicesStatus::black);
    attackScore = whiteScore - blackScore;
    board[whiteTargetPos.x][whiteTargetPos.y][whiteTargetPos.z] = chessPicesStatus::empty;

    //cout<<"attack score:"<<attackScore<<" defence score:"<<defenceScore<<endl;

    targetPos = whiteTargetPos;

    return attackScore;

}

int ChessBoard::isWin(int board[4][4][4])
{
    PicesFlatDataList flats;

    for(int y = 0;y < 4;y++)
    {
        for(int z = 0;z < 4;z++)
        {
            FlatsData data;
            data.a = board[0][y][z];
            data.b = board[1][y][z];
            data.c = board[2][y][z];
            data.d = board[3][y][z];
            flats.push_back(data);
        }
    }

    for(int x = 0;x < 4;x++)
    {
        for(int z = 0;z < 4;z++)
        {
            FlatsData data;
            data.a = board[x][0][z];
            data.b = board[x][1][z];
            data.c = board[x][2][z];
            data.d = board[x][3][z];
            flats.push_back(data);
        }
    }

    for(int x = 0;x < 4;x++)
    {
        for(int y = 0;y < 4;y++)
        {
            FlatsData data;
            data.a = board[x][y][0];
            data.b = board[x][y][1];
            data.c = board[x][y][2];
            data.d = board[x][y][3];
            flats.push_back(data);
        }
    }

    for(int y = 0;y < 4;y++)
    {
        FlatsData data;
        data.a = board[0][y][0];
        data.b = board[1][y][1];
        data.c = board[2][y][2];
        data.d = board[3][y][3];
        flats.push_back(data);
    }

    for(int x = 0;x < 4;x++)
    {
        FlatsData data;
        data.a = board[x][0][0];
        data.b = board[x][1][1];
        data.c = board[x][2][2];
        data.d = board[x][3][3];
        flats.push_back(data);
    }

    for(int y = 0;y < 4;y++)
    {
        FlatsData data;
        data.a = board[0][y][3];
        data.b = board[1][y][2];
        data.c = board[2][y][1];
        data.d = board[3][y][0];
        flats.push_back(data);
    }

    for(int x = 0;x < 4;x++)
    {
        FlatsData data;
        data.a = board[x][0][3];
        data.b = board[x][1][2];
        data.c = board[x][2][1];
        data.d = board[x][3][0];
        flats.push_back(data);
    }

    for(int z = 0;z < 4;z++)
    {
        FlatsData data;
        data.a = board[0][0][z];
        data.b = board[1][1][z];
        data.c = board[2][2][z];
        data.d = board[3][3][z];
        flats.push_back(data);
    }

    for(int z = 0;z < 4;z++)
    {
        FlatsData data;
        data.a = board[3][0][z];
        data.b = board[2][1][z];
        data.c = board[1][2][z];
        data.d = board[0][3][z];
        flats.push_back(data);
    }

    FlatsData data;
    data.a = board[0][0][0];
    data.b = board[1][1][1];
    data.c = board[2][2][2];
    data.d = board[3][3][3];
    flats.push_back(data);

    data.a = board[0][0][3];
    data.b = board[1][1][2];
    data.c = board[2][2][1];
    data.d = board[3][3][0];
    flats.push_back(data);

    data.a = board[3][0][0];
    data.b = board[2][1][1];
    data.c = board[1][2][2];
    data.d = board[0][3][3];
    flats.push_back(data);

    data.a = board[3][0][3];
    data.b = board[2][1][2];
    data.c = board[1][2][1];
    data.d = board[0][3][0];
    flats.push_back(data);

    int whiteNum = 0,blackNum = 0;

    for(auto iter = flats.begin();iter != flats.end();iter++)
    {
        whiteNum = 0;
        blackNum = 0;

        if(iter->a == chessPicesStatus::black)
            blackNum++;
        else if(iter->a == chessPicesStatus::white)
            whiteNum++;

        if(iter->b == chessPicesStatus::black)
            blackNum++;
        else if(iter->b == chessPicesStatus::white)
            whiteNum++;

        if(iter->c == chessPicesStatus::black)
            blackNum++;
        else if(iter->c == chessPicesStatus::white)
            whiteNum++;

        if(iter->d == chessPicesStatus::black)
            blackNum++;
        else if(iter->d == chessPicesStatus::white)
            whiteNum++;

        if(whiteNum == 4)
            return chessPicesStatus::white;

        if(blackNum == 4)
            return chessPicesStatus::black;

    }

    //cout<<"whiteNum:"<<whiteNum<<" blackNum:"<<blackNum<<endl;
    return chessPicesStatus::empty;

}

PicesPosList ChessBoard::takeback(int board[4][4][4], int step)
{
    PicesPosList list;

    if(history.size() < step)
        return list;

    for(int i = 0; i < step;i++)
    {
        PicesPos pos = history.top();
        history.pop();
        board[pos.x][pos.y][pos.z] = chessPicesStatus::empty;
        list.push_back(pos);
    }

    return list;
}
