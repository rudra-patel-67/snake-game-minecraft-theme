#include <iostream>
#include <lib\raylib.h>
#include <deque>
#include <lib\raymath.h>
using namespace std;

Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

int cellSize = 30;
int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;

bool eventTriggered(double interval)
{
    double currenTime= GetTime();
    if(currenTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currenTime;
        return true;
    }
    return false;
}

bool elementInDeque(Vector2 element, deque<Vector2> deque)
{
    for(unsigned int i = 0; i < deque.size(); i++)
    {
        if(Vector2Equals(deque[i],element))
        {
            return true;
        }
    }
    return false;
}

class Snake
{
    public:
        deque<Vector2> body = {Vector2{6,12},Vector2{5,12},Vector2{4,12}};
        Vector2 direction = {1,0};
        Texture2D texture, headTexture;
        bool addSegment = false;
        
        Snake()
        {
            Image image = LoadImage("assets/zombie.png");
            texture = LoadTextureFromImage(image);
            UnloadImage(image);
            image = LoadImage("assets/head.png");
            headTexture = LoadTextureFromImage(image);
            UnloadImage(image);
        }

        ~Snake()
        {
            UnloadTexture(texture);
            UnloadTexture(headTexture);
        }

        void draw()
        {
            for(unsigned int i=0 ; i<body.size();i++)
            {
                int x = body[i].x;
                int y = body[i].y;
                if(i==0)
                DrawTexture(headTexture,offset + x*cellSize,offset + y*cellSize,WHITE);
                else
                DrawTexture(texture,offset + x*cellSize,offset + y*cellSize,WHITE);
            }
        }

        void update()
        {
            body.push_front(Vector2Add(body[0],direction));
            if(addSegment)
            addSegment=false;
            else
            body.pop_back();
        }
};

class Food 
{
    public:
    Vector2 position;
    Texture2D texture;
    Food(deque<Vector2> snakeBody)
    {
        Image image = LoadImage("assets/steve-food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position=genRandomPos(snakeBody);
    }

    ~Food()
    {
        UnloadTexture(texture);
    }

    void draw() 
    {
        DrawTexture(texture,offset + position.x*cellSize, offset + position.y*cellSize,WHITE);
    }

    Vector2 genRandomPos(deque<Vector2> snakeBody)
    {
        float x,y;
        do
        {
            x = GetRandomValue(0,cellCount-1);
            y = GetRandomValue(0,cellCount-1);
            position = {x,y};
        }
        while(elementInDeque(position,snakeBody));
        Vector2 position = {x,y};
        return position;
    }
};

class Game
{
    public: 
    Food food = Food(snake.body);
    Snake snake = Snake();
    Sound s_Eat = LoadSound("assets/audio/eating.mp3");
    Sound s_Wall = LoadSound("assets/audio/hit-the-wall.mp3");
    bool gameOver=false;
    int score=0;

    void Draw()
    {
        if(!gameOver)
        {
            // snake.draw();
            // food.draw();
        }
    }

    void Update()
    {
        if(!gameOver)
        {
            snake.update();
            checkWall();
            checkEating();
            checkSelfCollision();
        }
    }

    void checkEating()
    {
        if(Vector2Equals(snake.body[0],food.position))
        {
            PlaySound(s_Eat);
            food.position = food.genRandomPos(snake.body);
            snake.addSegment = true;
            score++;
        }
    }

    void checkWall()
    {
        if(snake.body[0].x == cellCount || snake.body[0].x == -1 || snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            PlaySound(s_Wall);
            gameOver=true;
            cout<<endl<<"Hit the world border";
            displayScore();
        }
    }

    void checkSelfCollision()
    {
        deque<Vector2> headLessBody = snake.body;
        headLessBody.pop_front();
        if(elementInDeque(snake.body[0],headLessBody))
        {
            gameOver=true;
            cout<<endl<<"Self Collision";
            displayScore();
        }
    }

    void displayScore()
    {
        cout<<"\nYour Score is "<<score<<endl;
        cout<<"Press R to Restart\n";
        cout<<"Press Esc to Exit\n";
    }

    void reset()
    {
        snake.body = {Vector2{6,12},Vector2{5,12},Vector2{4,12}};
        snake.direction = {1,0};
        food.position = food.genRandomPos(snake.body);
        gameOver=false;
        score=0;
    }
};

int main() 
{
    InitWindow(2*offset + cellSize*cellCount, 2*offset + cellSize*cellCount, "Snake Game");
    InitAudioDevice();
    SetTargetFPS(60);

    Game game = Game();
    Music bgm = LoadMusicStream("assets/audio/bgm.mp3");
    PlayMusicStream(bgm);
    float vol = 1;

    while (WindowShouldClose() == false)
    {
        BeginDrawing();
        UpdateMusicStream(bgm);
        
        if(eventTriggered(0.2))
        {
            game.Update(); 
            // cout<<game.snake.body[0].x<<" "<<game.snake.body[1].y<<endl;              //FOR DEBUGGING
        }

        switch (GetKeyPressed())
        {
            case (KEY_UP):
            case (KEY_W):
                if(game.snake.direction.y !=1)
                game.snake.direction = {0,-1};
                break;
            
            case (KEY_DOWN):
            case (KEY_S):
                if(game.snake.direction.y !=-1)
                game.snake.direction = {0,1};
                break;
            
            case (KEY_RIGHT):
            case (KEY_D):
                if(game.snake.direction.x !=-1)
                game.snake.direction = {1,0};
                break;
            
            case (KEY_LEFT):
            case (KEY_A):
                if(game.snake.direction.x !=1)
                game.snake.direction = {-1,0};
                break;
            
            case (KEY_R):
                game.reset();
                break;

            default:
                break;
        }

        //Drawing
        if(!game.gameOver)
        {
            ClearBackground(green);
            DrawRectangleLinesEx(Rectangle{(float)offset-5,(float)offset-5,(float)cellSize*cellCount+10,(float)cellSize*cellCount+10},5,darkGreen);
            DrawText("Zombie Seige x Minecraft", offset - 5, 20, 40, darkGreen);
            DrawText(TextFormat("%i",game.score),offset - 5, offset + cellSize*cellCount+10, 40, darkGreen);
            game.Draw();
        }
        
        if(IsKeyPressed(KEY_R))
        game.reset();
        if(IsKeyPressed(KEY_O))
        game.gameOver=true;
 
        //Volume Controls   
        if(IsKeyPressed(KEY_KP_ADD) && vol < 1)
        {
            vol+=0.10;
            SetMusicVolume(bgm,vol);
        }
        if(IsKeyPressed(KEY_KP_SUBTRACT) && vol >= 0 )
        {
            vol-=0.10;
            SetMusicVolume(bgm,vol);
        }
        
        if(game.gameOver)
        {
            DrawText("Game Over", cellSize*(cellCount/2.5),  offset + cellSize*(cellCount/2.5), 60, darkGreen);
            DrawText("Press R to Restart", cellSize*(cellCount/2.3)+5,  offset + cellSize*(cellCount/2.5)+70, 25, darkGreen);
            DrawText("Press Esc to Quit", cellSize*(cellCount/2.2)+5,  offset + cellSize*(cellCount/2.5)+95, 25, darkGreen);
        }

        EndDrawing();    
    }
    StopMusicStream(bgm);
    UnloadMusicStream(bgm);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}