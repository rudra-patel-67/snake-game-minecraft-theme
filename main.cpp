#include <iostream>
#include <lib\raylib.h>
#include <deque>
#include <lib\raymath.h>
using namespace std;

Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

int cellSize = 30;
int cellCount = 25;

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
                DrawTexture(headTexture,x*cellSize,y*cellSize,WHITE);
                else
                DrawTexture(texture,x*cellSize,y*cellSize,WHITE);
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
        DrawTexture(texture,position.x*cellSize,position.y*cellSize,WHITE);
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
    int score=0;
    public: 
    Food food = Food(snake.body);
    Snake snake = Snake();
    bool gameOver=false;

    void Draw()
    {
        if(!gameOver)
        {
            snake.draw();
            food.draw();
        }
    }

    void Update()
    {
        snake.update();
        checkWall();
        checkEating();
    }

    void checkEating()
    {
        if(Vector2Equals(snake.body[0],food.position))
        {
            food.position = food.genRandomPos(snake.body);
            snake.addSegment = true;
            score++;
        }
    }

    void checkWall()
    {
        if(snake.body[0].x == cellCount || snake.body[0].x == -1 || snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            gameOver=true;
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
    InitWindow(cellSize*cellCount, cellSize*cellCount, "Snake Game");
    SetTargetFPS(60);

    Game game = Game();

    // KeyboardKey key;
    while (WindowShouldClose() == false)
    {
        BeginDrawing();

        if(eventTriggered(0.2))
        {
            game.Update(); 
            // cout<<game.snake.body[0].x<<" "<<game.snake.body[1].y<<endl;              //FOR DEBUGGING
        }
        // key = GetKeyPressed();
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
            game.Draw();
        }
        if(IsKeyPressed(KEY_R))
        game.reset();
        EndDrawing();    
    }
    CloseWindow();
    return 0;
}