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

int centerOf(int centerOfThis, char *text,int fSize)
{   
    int length = MeasureText(text,fSize);
    return centerOfThis-(length/2);
}

class Snake
{
    public:
        deque<Vector2> body = {Vector2{6,17},Vector2{5,17},Vector2{4,17}};
        Vector2 direction = {1,0};
        Texture2D texture, headTexture;
        int segmentToAdd=0;
        int segmentAdded=0;
        float interpolationFactor = 0.0f; // For smooth movement
        Vector2 nextHeadPosition; // To store the position of the next head
        
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

        // void update()
        // {
        //     // for(int i = 1 ; i <= segmentToAdd ; i++)
        //     if(segmentAdded<segmentToAdd && eventTriggered(0.2)){
        //         body.emplace_back(Vector2Add(body[0],direction));
        //         segmentAdded++;
        //         return;
        //     }        
        //     if(segmentAdded>=segmentToAdd)
        //     segmentToAdd=0;
        //     Vector2 temp1 = body[0];
        //     Vector2 temp2;
        //     body[0].x+=direction.x;
        //     body[0].y+=direction.y;
        //     for(unsigned int i=1;i<body.size();i++)
        //     {
        //             temp2=body[i];
        //             body[i]=temp1;
        //             temp1=temp2;
        //     }
        //     // free(&temp2);
        // }

        void update()
        {
            // Move the snake's head in the current direction
            Vector2 newHead = Vector2Add(body[0], direction);
            // Add the new head to the front
            body.push_front(newHead);
            // Handle the addition of new segments
            if (segmentToAdd > 0)
            {
                segmentToAdd--;  // Reduce the number of segments left to add
            }
            else
            {
                // Remove the last segment if no growth is needed
                body.pop_back();
            }
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
        if(count==0)
        DrawTextureEx(steveFTexture, Vector2Add({(float)offset-5, (float)offset-5}, Vector2Scale(position, (float)cellSize)), 0, 1.1, WHITE);
        else
        DrawTextureEx(villagerFTexture, Vector2Add({(float)offset, (float)offset}, Vector2Scale(position, (float)cellSize)), 0, 0.80, WHITE);
            // DrawTextureEx(steveFTexture, Vector2Add({(float)offset, (float)offset}, Vector2Scale(position, (float)cellSize)), 0, 1.1, WHITE);
        // DrawTexture(villagerFTexture,offset + position.x*cellSize, offset + position.y*cellSize,WHITE);
        // DrawTextureEx(villagerFTexture, Vector2Add({(float)offset, (float)offset}, Vector2Scale(position, (float)cellSize)), 0, 0.75, WHITE);
    }

    void superFood()
    {
        ateSuperFood=(count==0)?true:false;
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
    int timer = 0;
    
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
            food.superFood();
            checkWall();
            checkEating();
            checkSelfCollision();
        }
    }

    // void snakeUpdate()
    // {
    //     if(eventTriggered(0.1))
    // }

    void checkEating()
    {
        if(Vector2Equals(snake.body[0],food.position))
        {
            PlaySound(s_Eat);
            if(food.ateSuperFood)
            {
                // food.count=1;
                snake.segmentToAdd=5;
                // snake.update();
                score+=5;
            }
            else
            {
                snake.segmentToAdd = 1;
                score++;
                // food.count++;
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

void movement(Game &game)
{
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
        
        case (KEY_O):
            game.gameOver=true;
            break;

        default:
            break;
    }
}

int main() 
{
    int fps=GetFPS();
    InitWindow(2*offset + playground, 2*offset + playground, TextFormat("Snake Game \t FPS : %d",fps));
    InitAudioDevice();
    // SetTargetFPS(1000);
    int frames = 0;
    bool gamePaused = false;
    Game game = Game();
    Music bgm = LoadMusicStream("assets/audio/bgm.ogg");
    Image wall = LoadImageAnim("assets/wall.gif",&frames);
    Image volumeImg[4];
    const char *imgAssets[] = {"assets/volMute.png","assets/lowVol.png","assets/halfVol.png","assets/fullVol.png"};
    Texture2D volTexture[4];
    char title[]= "Zombie Siege x Minecraft";
    
    //Loading volume icon texture
    for(int i=0;i<4;i++){
        volumeImg[i]=LoadImage(imgAssets[i]);
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
        if(IsKeyPressed(KEY_P)) gamePaused=!gamePaused;
        
        BeginDrawing();
        ClearBackground(green);

        if(!gamePaused){
            timer += GetFrameTime();  // Increment timer by frame time

            //Wall Animation
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

            movement(game);
            if(eventTriggered(0.2) && !gamePaused)
            game.Update(); 
            // game.snakeUpdate();
            // {
                // cout<<game.snake.body[0].x<<" "<<game.snake.body[1].y<<endl;              //FOR DEBUGGING
            // }
            
        }

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

        //Drawing
        if(!game.gameOver)
            game.Draw();
        DrawRectangleLinesEx(Rectangle{(float)offset-5,(float)offset-5,(float)playground+10,(float)playground+10},5,darkGreen);
        DrawRectangleLinesEx(Rectangle{(float)offset-cellSize,(float)offset-cellSize,(float)playground+cellSize*2,(float)playground+cellSize*2},5,darkGreen);
        DrawText(title, centerOf(center,title,35), 8, 35, darkGreen);
        DrawText(TextFormat("%i",game.score),offset - cellSize + 5, offset + playground+cellSize+3, 40, darkGreen);
        DrawTexture(((volPercentage==0)?volTexture[0]:(volPercentage>0&&volPercentage<40)?volTexture[1]:(volPercentage>=40&&volPercentage<70)?volTexture[2]:volTexture[3]),playground,playground+offset+10+cellSize,WHITE);
        DrawText(TextFormat(" : %g %",copysign((ceilf(vol*100)),1.0f)), playground+cellSize, playground+offset+10+cellSize, 25, darkGreen);
        
        fps=GetFPS();
        SetWindowTitle(TextFormat("Snake Game \t FPS : %d",fps));

        //Game Over Screen
        if(game.gameOver)
        {
            DrawText("Game Over",centerOf(center,(char *)"Game Over", 60), center - 60, 60, darkGreen);
            DrawText("Press R to Restart", centerOf(center, (char *)"Press R to Restart",25), center + 5, 25, darkGreen);
            DrawText("Press Esc to Quit", centerOf(center,(char *)"Press Esc to Quit",25), center + 35, 25, darkGreen);
        }

        //Wall Rendering
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
