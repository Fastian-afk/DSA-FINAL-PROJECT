#include "ScoreBoard.h"
// Initialize static member
ScoreBoard* ScoreBoard::instance = nullptr;
// Most of the implementation is in the header file since we're using templates
// and inline functions. This file is kept for future expansion if needed. 