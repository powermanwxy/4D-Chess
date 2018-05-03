#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include <vector>
#include <QObject>

/**
 * @brief Chess Pices Status
 *
 */
enum chessPicesStatus{empty, black, white, air};
/**
 * @brief Single pices status
 *
 */
struct PicesPos{
    int x; /**< X pos */
    int y; /**< Y pos */
    int z; /**< z pos */
    chessPicesStatus type; /**< Pices Status */
    int value; /**< Position value */
};

/**
 * @brief Flat data group
 *
 */
struct FlatsData{
    int a; /**< TODO: describe */
    int b; /**< TODO: describe */
    int c; /**< TODO: describe */
    int d; /**< TODO: describe */
};

/**
 * @brief Pices Position List
 *
 */
typedef std::vector<PicesPos> PicesPosList;
/**
 * @brief Flat Data List
 *
 */
typedef std::vector<FlatsData> PicesFlatDataList;

/**
 * @brief Logic class
 *
 */
class ChessBoard : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Create instance.
     *
     * @param parent
     */
    explicit ChessBoard(QObject *parent = 0);

    int chessBoard[4][4][4]; /**< Chess Board x,y,z */
    PicesPos whiteTargetPos; /**< Pridect white pices target pos */
    PicesPos blackTargetPos; /**< Pridect black pices target pos */
    PicesPos targetPos; /**< Result pos, this result is after compire between attach or defence */

    /**
     * @brief Init and clear borad.
     *
     */
    void init();
    /**
     * @brief Insert a pices.
     *
     * @param x
     * @param y
     * @param type
     * @return The pos is a valid pos or not.
     */
    bool insertPices(int x,int y,int type);
    /**
     * @brief Print the chess board status.
     *
     */
    void printBoard();

    /**
     * @brief Return a list of avliable pos, including pos and score.
     *
     * @param board[][][]
     * @param side
     * @return PicesPosList
     */
    PicesPosList getAvailablePos(int board[4][4][4],chessPicesStatus side);
    /**
     * @brief Return position's value.
     *
     * @param board[][][]
     * @param pos
     * @param side
     * @return int
     */
    int getPosValue(int board[4][4][4], PicesPos *pos,chessPicesStatus side);
    /**
     * @brief Return group(flat data)'s value.
     *
     * @param flats
     * @param status
     * @return int
     */
    int getFlatPicesValue(PicesFlatDataList flats, chessPicesStatus status);
    /**
     * @brief Return side's globe score.
     *
     * @param board[][][]
     * @param side
     * @return int
     */
    int getSideScore(int board[4][4][4], chessPicesStatus side);
    /**
     * @brief Main search pridect function.
     *
     * @param board[][][]
     * @param deep
     * @return int
     */
    int dfs(int board[4][4][4], int deep = 6);
    /**
     * @brief Search white pices' best steps.
     *
     * @param board[][][]
     * @param deep
     * @param alpha
     * @param beta
     * @return int
     */
    int findWhiteScoreMaxValue(int board[4][4][4],int deep,int alpha,int beta);
    /**
     * @brief Search black pices' best steps.
     *
     * @param board[][][]
     * @param deep
     * @param alpha
     * @param beta
     * @return int
     */
    int findWhiteScoreMinValue(int board[4][4][4],int deep,int alpha,int beta);
    /**
     * @brief Return is anyone win.
     *
     * @param board[][][]
     * @return int
     */
    int isWin(int board[4][4][4]);
    /**
     * @brief Take back.
     *
     * @param board[][][]
     * @param step
     * @return PicesPosList
     */
    PicesPosList takeback(int board[4][4][4], int step);

private:

signals:

public slots:
};

#endif // CHESSBOARD_H
