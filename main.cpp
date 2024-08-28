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
int playground = cellSize * cellCount;
int center = offset + (playground/2);
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
        deque<Vector2> body = {Vector2{6,17},Vector2{5,17},Vector2{4,17}};
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
    Texture2D villagerFTexture;
    Texture2D steveFTexture;
    int count=0;
    bool ateSuperFood = false;

    Food(deque<Vector2> snakeBody)
    {
        Image villagerFood = LoadImage("assets/villager-food.png");
        Image steveFood = LoadImage("assets/steve-food.png");
        villagerFTexture = LoadTextureFromImage(villagerFood);
        steveFTexture = LoadTextureFromImage(steveFood);
        UnloadImage(villagerFood);
        UnloadImage(steveFood);
        position=genRandomPos(snakeBody);
    }

    ~Food()
    {
        UnloadTexture(villagerFTexture);
        UnloadTexture(steveFTexture);

    }

    void draw() 
    {
        if(count==5)
        DrawTextureEx(steveFTexture, Vector2Add({(float)offset-5, (float)offset-5}, Vector2Scale(position, (float)cellSize)), 0, 1.1, WHITE);
        else
        DrawTextureEx(villagerFTexture, Vector2Add({(float)offset, (float)offset}, Vector2Scale(position, (float)cellSize)), 0, 0.80, WHITE);
            // DrawTextureEx(steveFTexture, Vector2Add({(float)offset, (float)offset}, Vector2Scale(position, (float)cellSize)), 0, 1.1, WHITE);
        // DrawTexture(villagerFTexture,offset + position.x*cellSize, offset + position.y*cellSize,WHITE);
        // DrawTextureEx(villagerFTexture, Vector2Add({(float)offset, (float)offset}, Vector2Scale(position, (float)cellSize)), 0, 0.75, WHITE);
    }

    

    void superFood()
    {
        ateSuperFood=(count==5)?true:false;
    
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
    Sound s_Eat = LoadSound("assets/audio/eating.ogg");
    Sound s_Wall = LoadSound("assets/audio/hit-the-wall.ogg");
    bool gameOver=false;
    int score=0;
    int highScore;

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
        if(!gameOver)
        {
            snake.update();
            checkWall();
            checkEating();
            checkSelfCollision();
            food.superFood();
        }
    }

    void checkEating()
    {
        if(Vector2Equals(snake.body[0],food.position))
        {
            PlaySound(s_Eat);
            if(food.ateSuperFood)
            {
                food.count=0;
                for(int i=0 ; i<3 ; i++)
                {
                    snake.addSegment = true;
                    snake.update();
                }
                score+=3;
            }
            else
            {
                snake.addSegment = true;
                score++;
                food.count++;
            }
            food.position = food.genRandomPos(snake.body);
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
        food.count=0;
    }
};

int main() 
{
    InitWindow(2*offset + playground, 2*offset + playground, "Snake Game");
    InitAudioDevice();
    SetTargetFPS(60);
    int frames = 0;
    Game game = Game();
    Music bgm = LoadMusicStream("assets/audio/bgm.ogg");
    Image wall = LoadImageAnim("assets/wall.gif",&frames);
    Image volumeImg[4];
    volumeImg[0]=LoadImage("assets/volMute.png");
    volumeImg[1]=LoadImage("assets/lowVol.png");
    volumeImg[2]=LoadImage("assets/halfVol.png");
    volumeImg[3]=LoadImage("assets/fullVol.png");

    Texture2D volTexture[4];
    for(int i=0; i<4 ; i++)
    {
        volTexture[i] = LoadTextureFromImage(volumeImg[i]);
        UnloadImage(volumeImg[i]);
    }


    Texture2D wallTexture = LoadTextureFromImage(wall);

    int currentFrame = 0;        // Index of the current frame
    float frameTime = 0.1f;      // Time in seconds to display each frame
    float timer = 0.0f;

    PlayMusicStream(bgm);
    float vol = 1;
    float volPercentage;

    while (WindowShouldClose() == false)
    {
        BeginDrawing();
        ClearBackground(green);

        timer += GetFrameTime();  // Increment timer by frame time

        if (timer >= frameTime) {
            // Move to the next frame
            currentFrame++;
            timer = 0.0f;  // Reset timer

            if (currentFrame >= frames) currentFrame = 0;  // Loop animation

            // Update texture data to current frame (no need to reload the texture completely)
            UpdateTexture(wallTexture, (unsigned char *)wall.data + currentFrame * wall.width * wall.height * 4);
        }
        UpdateMusicStream(bgm);
        volPercentage = copysign((ceilf(vol*100)),1.0f);

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
            game.Draw();
        }
        DrawRectangleLinesEx(Rectangle{(float)offset-5,(float)offset-5,(float)playground+10,(float)playground+10},5,darkGreen);
        DrawRectangleLinesEx(Rectangle{(float)offset-cellSize,(float)offset-cellSize,(float)playground+cellSize*2,(float)playground+cellSize*2},5,darkGreen);
        DrawText("Zombie Siege x Minecraft", offset - cellSize, 8, 35, darkGreen);
        DrawText(TextFormat("%i",game.score),offset - cellSize + 5, offset + playground+cellSize+3, 40, darkGreen);
        DrawTexture(((volPercentage==0)?volTexture[0]:(volPercentage>0&&volPercentage<40)?volTexture[1]:(volPercentage>=40&&volPercentage<70)?volTexture[2]:volTexture[3]),playground,playground+offset+10+cellSize,WHITE);
        DrawText(TextFormat(" : %g %",copysign((ceilf(vol*100)),1.0f)), playground+cellSize, playground+offset+10+cellSize, 25, darkGreen);
        
        if(IsKeyPressed(KEY_R))
        game.reset();
        if(IsKeyPressed(KEY_O))
        game.gameOver=true;
 
        //Volume Controls   
        float tempVol;
        if(IsKeyPressed(KEY_KP_ADD) && vol < 1)
        {
            vol+=0.05;
            SetMusicVolume(bgm,vol);
            SetSoundVolume(game.s_Eat,vol);
            SetSoundVolume(game.s_Wall,vol);
        }
        if(IsKeyPressed(KEY_KP_SUBTRACT) && vol > 0.00 )
        {
            vol-=0.05;
            SetMusicVolume(bgm,vol);
            SetSoundVolume(game.s_Eat,vol);
            SetSoundVolume(game.s_Wall,vol);
        }
        if(IsKeyPressed(KEY_M) && vol > 0)
        {
            tempVol = vol;
            vol = 0;
            SetMusicVolume(bgm,vol);
            SetSoundVolume(game.s_Eat,vol);
            SetSoundVolume(game.s_Wall,vol);
        }
        else if(vol==0 && IsKeyPressed(KEY_M))
        {
            vol = tempVol;
            SetMusicVolume(bgm,vol);
            SetSoundVolume(game.s_Eat,vol);
            SetSoundVolume(game.s_Wall,vol);
        }


        if(game.gameOver)
        {
            DrawText("Game Over",center-offset*2, center - 60, 60, darkGreen);
            DrawText("Press R to Restart", center-offset*1.5, center + 5, 25, darkGreen);
            DrawText("Press Esc to Quit", center-offset*1.35, center + 35, 25, darkGreen);
        }

        for(int i = 0; i<=cellCount+1; i++)
        {
            for(int j = 0; j<=cellCount+1; j++)
            {
                if(i==0 || j == 0 || i==cellCount+1 || j == cellCount+1)
                DrawTexture(wallTexture, offset-cellSize+(i*cellSize), offset-cellSize+(j*cellSize), WHITE);
            }
        }

        EndDrawing();    
    }
    StopMusicStream(bgm);
    UnloadMusicStream(bgm);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
