#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

namespace
{
    bool Debug_Output = false;

    float Field_W = 800.0f;
    float Field_H = 480.0f;
    float Paddle_W = 16.0f;
    float Paddle_H = 92.0f;
    float Ball_Size = 14.0f;
    float Left_X = 36.0f;
    float Right_X = 748.0f;
    float Left_Y = 194.0f;
    float Right_Y = 194.0f;
    float Ball_X = 393.0f;
    float Ball_Y = 233.0f;
    float Ball_VX = 5.2f;
    float Ball_VY = 3.2f;
    int Left_Dir = 0;
    int Right_Dir = 0;
    int Left_Score = 0;
    int Right_Score = 0;

    void Print(const std::string &Message)
    {
        if(Debug_Output) std::cout << Message << std::endl;
    }

    bool Intersects(float AX, float AY, float AW, float AH, float BX, float BY, float BW, float BH)
    {
        return AX < BX + BW && AX + AW > BX && AY < BY + BH && AY + AH > BY;
    }

    void Reset_Ball(float Direction)
    {
        Ball_X = Field_W * 0.5f - Ball_Size * 0.5f;
        Ball_Y = Field_H * 0.5f - Ball_Size * 0.5f;
        Ball_VX = 5.2f * Direction;
        Ball_VY = 3.0f;
    }
}

extern "C" void Constructor() { Print("Bananas_Pong Constructor()"); }
extern "C" void Destructor() { Print("Bananas_Pong Destructor()"); }
extern "C" void Debug(bool Enable) { Debug_Output = Enable; }

extern "C" void Reset()
{
    Left_Y = Field_H * 0.5f - Paddle_H * 0.5f;
    Right_Y = Left_Y;
    Left_Score = 0;
    Right_Score = 0;
    Reset_Ball(1.0f);
}

extern "C" void Set_Field(float Width, float Height)
{
    Field_W = Width;
    Field_H = Height;
    Right_X = Field_W - 52.0f;
    Reset();
}

extern "C" void Left_Up() { Left_Dir = -1; }
extern "C" void Left_Down() { Left_Dir = 1; }
extern "C" void Left_Stop() { Left_Dir = 0; }
extern "C" void Right_Up() { Right_Dir = -1; }
extern "C" void Right_Down() { Right_Dir = 1; }
extern "C" void Right_Stop() { Right_Dir = 0; }

extern "C" void Update()
{
    const float Paddle_Speed = 7.0f;
    Left_Y += Left_Dir * Paddle_Speed;

    if(Right_Dir == 0)
    {
        float Right_Center = Right_Y + Paddle_H * 0.5f;
        float Ball_Center = Ball_Y + Ball_Size * 0.5f;
        if(Ball_Center < Right_Center - 8.0f) Right_Y -= Paddle_Speed * 0.82f;
        if(Ball_Center > Right_Center + 8.0f) Right_Y += Paddle_Speed * 0.82f;
    }
    else
    {
        Right_Y += Right_Dir * Paddle_Speed;
    }

    Left_Y = std::clamp(Left_Y, 0.0f, Field_H - Paddle_H);
    Right_Y = std::clamp(Right_Y, 0.0f, Field_H - Paddle_H);

    Ball_X += Ball_VX;
    Ball_Y += Ball_VY;

    if(Ball_Y <= 0.0f)
    {
        Ball_Y = 0.0f;
        Ball_VY = std::fabs(Ball_VY);
    }
    else if(Ball_Y + Ball_Size >= Field_H)
    {
        Ball_Y = Field_H - Ball_Size;
        Ball_VY = -std::fabs(Ball_VY);
    }

    if(Intersects(Ball_X, Ball_Y, Ball_Size, Ball_Size, Left_X, Left_Y, Paddle_W, Paddle_H) && Ball_VX < 0.0f)
    {
        float Offset = ((Ball_Y + Ball_Size * 0.5f) - (Left_Y + Paddle_H * 0.5f)) / (Paddle_H * 0.5f);
        Ball_X = Left_X + Paddle_W;
        Ball_VX = std::fabs(Ball_VX) + 0.18f;
        Ball_VY = Offset * 5.2f;
    }
    else if(Intersects(Ball_X, Ball_Y, Ball_Size, Ball_Size, Right_X, Right_Y, Paddle_W, Paddle_H) && Ball_VX > 0.0f)
    {
        float Offset = ((Ball_Y + Ball_Size * 0.5f) - (Right_Y + Paddle_H * 0.5f)) / (Paddle_H * 0.5f);
        Ball_X = Right_X - Ball_Size;
        Ball_VX = -std::fabs(Ball_VX) - 0.18f;
        Ball_VY = Offset * 5.2f;
    }

    if(Ball_X + Ball_Size < 0.0f)
    {
        Right_Score++;
        Reset_Ball(1.0f);
    }
    else if(Ball_X > Field_W)
    {
        Left_Score++;
        Reset_Ball(-1.0f);
    }
}

extern "C" float Left_X_Value() { return Left_X; }
extern "C" float Left_Y_Value() { return Left_Y; }
extern "C" float Right_X_Value() { return Right_X; }
extern "C" float Right_Y_Value() { return Right_Y; }
extern "C" float Paddle_W_Value() { return Paddle_W; }
extern "C" float Paddle_H_Value() { return Paddle_H; }
extern "C" float Ball_X_Value() { return Ball_X; }
extern "C" float Ball_Y_Value() { return Ball_Y; }
extern "C" float Ball_Size_Value() { return Ball_Size; }
extern "C" int Left_Score_Value() { return Left_Score; }
extern "C" int Right_Score_Value() { return Right_Score; }
