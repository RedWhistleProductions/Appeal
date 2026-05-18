#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

namespace
{
    constexpr int Rows = 5;
    constexpr int Cols = 10;
    constexpr int Brick_Count = Rows * Cols;

    bool Debug_Output = false;
    float Field_W = 800.0f;
    float Field_H = 520.0f;
    float Paddle_W = 112.0f;
    float Paddle_H = 16.0f;
    float Paddle_X = 344.0f;
    float Paddle_Y = 476.0f;
    float Ball_Size = 12.0f;
    float Ball_X = 394.0f;
    float Ball_Y = 430.0f;
    float Ball_VX = 4.3f;
    float Ball_VY = -4.6f;
    float Brick_W = 64.0f;
    float Brick_H = 22.0f;
    float Brick_Gap = 8.0f;
    float Brick_Start_X = 44.0f;
    float Brick_Start_Y = 64.0f;
    int Paddle_Dir = 0;
    int Score = 0;
    int Lives = 3;
    int Bricks[Brick_Count] = {};
    std::string Last_Event;

    void Print(const std::string &Message)
    {
        if(Debug_Output) std::cout << Message << std::endl;
    }

    int Index(int Row, int Col)
    {
        return Row * Cols + Col;
    }

    bool Intersects(float AX, float AY, float AW, float AH, float BX, float BY, float BW, float BH)
    {
        return AX < BX + BW && AX + AW > BX && AY < BY + BH && AY + AH > BY;
    }

    void Reset_Ball()
    {
        Ball_X = Paddle_X + Paddle_W * 0.5f - Ball_Size * 0.5f;
        Ball_Y = Paddle_Y - 28.0f;
        Ball_VX = 4.2f;
        Ball_VY = -4.8f;
    }

    void Reset_Level()
    {
        for(int i = 0; i < Brick_Count; ++i)
        {
            Bricks[i] = 1;
        }
    }
}

extern "C" void Constructor() { Print("Bananas_Breakout Constructor()"); }
extern "C" void Destructor() { Print("Bananas_Breakout Destructor()"); }
extern "C" void Debug(bool Enable) { Debug_Output = Enable; }

extern "C" void Reset()
{
    Paddle_X = Field_W * 0.5f - Paddle_W * 0.5f;
    Paddle_Y = Field_H - 44.0f;
    Paddle_Dir = 0;
    Score = 0;
    Lives = 3;
    Reset_Level();
    Reset_Ball();
}

extern "C" void Set_Field(float Width, float Height)
{
    Field_W = Width;
    Field_H = Height;
    Brick_Start_X = 44.0f;
    Brick_Start_Y = 64.0f;
    Reset();
}

extern "C" void Left() { Paddle_Dir = -1; }
extern "C" void Right() { Paddle_Dir = 1; }
extern "C" void Stop() { Paddle_Dir = 0; }

extern "C" void Update()
{
    Last_Event.clear();
    const float Paddle_Speed = 8.0f;
    Paddle_X += Paddle_Dir * Paddle_Speed;
    Paddle_X = std::clamp(Paddle_X, 0.0f, Field_W - Paddle_W);

    Ball_X += Ball_VX;
    Ball_Y += Ball_VY;

    if(Ball_X <= 0.0f)
    {
        Ball_X = 0.0f;
        Ball_VX = std::fabs(Ball_VX);
        Last_Event = "Wall";
    }
    else if(Ball_X + Ball_Size >= Field_W)
    {
        Ball_X = Field_W - Ball_Size;
        Ball_VX = -std::fabs(Ball_VX);
        Last_Event = "Wall";
    }

    if(Ball_Y <= 0.0f)
    {
        Ball_Y = 0.0f;
        Ball_VY = std::fabs(Ball_VY);
        Last_Event = "Wall";
    }

    if(Intersects(Ball_X, Ball_Y, Ball_Size, Ball_Size, Paddle_X, Paddle_Y, Paddle_W, Paddle_H) && Ball_VY > 0.0f)
    {
        float Offset = ((Ball_X + Ball_Size * 0.5f) - (Paddle_X + Paddle_W * 0.5f)) / (Paddle_W * 0.5f);
        Ball_Y = Paddle_Y - Ball_Size;
        Ball_VY = -std::fabs(Ball_VY);
        Ball_VX = Offset * 6.0f;
        Last_Event = "Paddle";
    }

    for(int Row = 0; Row < Rows; ++Row)
    {
        for(int Col = 0; Col < Cols; ++Col)
        {
            int Brick = Index(Row, Col);
            if(Bricks[Brick] == 0) continue;

            float X = Brick_Start_X + Col * (Brick_W + Brick_Gap);
            float Y = Brick_Start_Y + Row * (Brick_H + Brick_Gap);
            if(Intersects(Ball_X, Ball_Y, Ball_Size, Ball_Size, X, Y, Brick_W, Brick_H))
            {
                Bricks[Brick] = 0;
                Score += 10;
                Ball_VY = -Ball_VY;
                Last_Event = "Brick";
                return;
            }
        }
    }

    if(Ball_Y > Field_H)
    {
        Lives--;
        Last_Event = "Life";
        if(Lives <= 0)
        {
            Last_Event = "GameOver";
            Reset();
        }
        else
        {
            Reset_Ball();
        }
    }

    bool Any_Bricks = false;
    for(int i = 0; i < Brick_Count; ++i)
    {
        if(Bricks[i] != 0)
        {
            Any_Bricks = true;
            break;
        }
    }
    if(!Any_Bricks)
    {
        Reset_Level();
        Reset_Ball();
        Last_Event = "Level";
    }
}

extern "C" float Paddle_X_Value() { return Paddle_X; }
extern "C" float Paddle_Y_Value() { return Paddle_Y; }
extern "C" float Paddle_W_Value() { return Paddle_W; }
extern "C" float Paddle_H_Value() { return Paddle_H; }
extern "C" float Ball_X_Value() { return Ball_X; }
extern "C" float Ball_Y_Value() { return Ball_Y; }
extern "C" float Ball_Size_Value() { return Ball_Size; }
extern "C" int Score_Value() { return Score; }
extern "C" int Lives_Value() { return Lives; }
extern "C" std::string Event_Value() { return Last_Event; }
extern "C" void Clear_Event() { Last_Event.clear(); }
extern "C" int Brick_Alive(int Index) { return Index >= 0 && Index < Brick_Count ? Bricks[Index] : 0; }
extern "C" float Brick_X_Value(int Index) { return Brick_Start_X + (Index % Cols) * (Brick_W + Brick_Gap); }
extern "C" float Brick_Y_Value(int Index) { return Brick_Start_Y + (Index / Cols) * (Brick_H + Brick_Gap); }
extern "C" float Brick_W_Value() { return Brick_W; }
extern "C" float Brick_H_Value() { return Brick_H; }
