#include <stdio.h>
#include "raylib.h"
#define MAX_FRAME_SPEED 15
#define MIN_FRAME_SPEED 8
#define PLAYER_MAX_SHOOTS 3

#define G 450
#define PLAYER_JUMP_SPD 410.0f
#define PLAYER_HOR_SPD 200.0f

typedef struct Player {
    Vector2 position;
    float speed;
    bool canJump;
    int PlayerDirection;
    bool jumping;
    bool right;
    bool left;
    float HP;

} Player;

typedef struct block {
    Rectangle rect;
    int blocking;
    Color color;
    int type;

} block;

typedef struct Map {
    block blockMap[65000];

} Map;

typedef struct Shoot{
    Vector2 speed;
    bool active;
    Color color;
    Vector2 position;
    int lifeSpan;
    float radius;
    int dir;
    float limit;
} Shoot;


static Shoot shoot[PLAYER_MAX_SHOOTS] = {0};


Player player1 = {{350, 50},0,0,0,0,1,0,400};
Sound bulletSnd;

void UpdatePlayer(float delta);
void collisionPlayer(Map map, Rectangle mask, Player player);


int main(void)
{


    int framesCounter = 0;
    int Animspeed = 5;
    float xPOS= 0;
    int frame = 0;
    const int screenWidth = 1000;
    const int screenHeight = 600;


    InitWindow(screenWidth, screenHeight, "Terraria-Aaron-Jose-Bayro"); //crea la pantalla
    InitAudioDevice();


    Texture2D PlayerTxtr = LoadTexture("Assets/Person01.png");
    bulletSnd = LoadSound("Assets/BulletSnd.wav");


    Map Map1;                                                        // Generar terreno
    int posX = 0;
    int posY = 150;
    int cont =0;

    for (int x = 0; x < 100; x++) {
        for (int y = 0; y < 650 ; y++) {
            Map1.blockMap[cont].rect.x = posX;
            Map1.blockMap[cont].rect.y = posY;
            Map1.blockMap[cont].rect.width = 25;
            Map1.blockMap[cont].rect.height = 25;
            Map1.blockMap[cont].blocking= 1;
            Map1.blockMap[cont].color = GRAY;
            Map1.blockMap[cont].type =0;       /// TIPO DE BLOQUE

            if(x >= 0  && x <11){
                Map1.blockMap[cont].type =  1;
                //Texture2D blockDirt= LoadTexture("tierra.png"); ///INTENTO PONER TEXTURA A LA TIERRA
                Map1.blockMap[cont].color = BROWN;
            }
            if(x <= 100 && x >= 75){
                Map1.blockMap[cont].type = 2;
                Map1.blockMap[cont].color = RED;
            }

            posX = posX + 25;
            cont ++;
        }
        posY = posY + 25;
        posX= 10;
    }
    int MapLength = sizeof(Map1.blockMap)/sizeof(Map1.blockMap[0]); // Generar terreno


    Camera2D camera = { 0 };
    camera.target = player1.position;
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;


    for (int i = 0; i < PLAYER_MAX_SHOOTS; i++) {
        shoot[i].position = (Vector2) {player1.position.x+30, player1.position.y+30};
        shoot[i].speed.x = 3;
        shoot[i].radius = 3;
        shoot[i].active = false;
        shoot[i].color = RED;
        shoot[i].lifeSpan = 0;
        shoot[i].dir = 0;
        shoot[i].limit = player1.position.x+200;

    }

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {

        float deltaTime = GetFrameTime();
        UpdatePlayer(deltaTime);//Changes player state and movement


        //UpdateCameraCenter(camera, player1, deltaTime, screenWidth, screenHeight);


        framesCounter++;

        if (framesCounter>=Animspeed){                                  //ANIMATION
            framesCounter= 0;
            if(frame!=8){
                xPOS = (float)frame*(float)PlayerTxtr.width/9;
                frame += 1;
            } else {xPOS = 0; frame = 1;
            }
        }
        float maskHorizontal = PlayerTxtr.width/9;
        float maskVertical = PlayerTxtr.height/5;
        Rectangle mask;
        mask.x =xPOS;
        mask.y =player1.PlayerDirection*(float)PlayerTxtr.height/5;
        mask.height=maskVertical;
        mask.width=maskHorizontal;                                    //ANIMATION


        camera.offset = (Vector2){ screenWidth/2.0f - (PlayerTxtr.width/18), screenHeight/2.0f - (PlayerTxtr.height/5)};
        camera.target = player1.position;

        BeginDrawing();


        ClearBackground(RAYWHITE);

        BeginMode2D(camera);
        DrawTextureRec(PlayerTxtr, mask, player1.position, WHITE);


        for (int i = 0; i < PLAYER_MAX_SHOOTS; i++) {                     //DRAW BULLETS
            if(shoot[i].active){
                shoot[i].position.x += shoot[i].dir*8.5;
                shoot[i].lifeSpan++;
                if(shoot[i].active){
                    DrawCircleV(shoot[i].position, shoot[i].radius, RED);
                }
                if(shoot[i].lifeSpan >= 80){
                    shoot[i].position = (Vector2){player1.position.x+30, player1.position.y+30};
                    shoot[i].speed = (Vector2) {0,0};
                    shoot[i].lifeSpan = 0;
                    shoot[i].active = false;
                }
            }
        }                      //DIBUJAR BALAS

        for (int i = 0; i < MapLength; i++) DrawRectangleRec(Map1.blockMap[i].rect, Map1.blockMap[i].color);
        EndDrawing();

    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}

void UpdatePlayer(float delta){

    if(IsKeyDown(KEY_RIGHT)){
        player1.position.x += PLAYER_HOR_SPD*delta;
        player1.right = true;
        player1.left = false;
        player1.PlayerDirection=3;
    }
    if(IsKeyDown(KEY_LEFT)){
        player1.position.x -= PLAYER_HOR_SPD*delta;
        player1.left = true;
        player1.right = false;
        player1.PlayerDirection=1;
    }
    if (IsKeyDown(KEY_SPACE) )
    {
        player1.speed = -PLAYER_JUMP_SPD;
    }

    if(IsKeyUp(KEY_RIGHT) && IsKeyUp(KEY_LEFT) ){
        player1.PlayerDirection=2;
    }



    if(IsKeyPressed(KEY_W)){
        for (int i = 0; i < PLAYER_MAX_SHOOTS; i++) {
            if(!shoot[i].active){
                PlaySound(bulletSnd);
                shoot[i].position = (Vector2) {player1.position.x+30, player1.position.y+30};
                shoot[i].active = true;
                if(player1.right)shoot[i].dir =1;
                else if(player1.left)shoot[i].dir =-1;
                break;
            }
        }
    }


     player1.position.y += player1.speed*delta;                 //gravity
     player1.speed += G*delta;
}

void collisionPlayer(Map map, Rectangle mask, Player player){

    for (int y = 0; y < 100; ++y) {
        for (int x = 0; x < 650; ++x) {
            if (mask.x < map.blockMap[x].rect.x + map.blockMap->rect.width){
                if(mask.x < map.blockMap[x].rect.x + map.blockMap->rect.width &&
                mask.x + mask.width > map.blockMap[x].rect.x + map.blockMap->rect.width){
                    //deteccion derecha borde
                    mask.x = map.blockMap[x].rect.x + map.blockMap->rect.width;
                }
                else if (mask.x + mask.width > map.blockMap[x].rect.x && mask.x < map.blockMap[x].rect.x){
                    //deteccion izquierda borde
                    mask.x = map.blockMap[x].rect.x - map.blockMap->rect.width;
                }
            }
            if(mask.x + mask.width > map.blockMap[x].rect.x && mask.x < map.blockMap[x].rect.x + map.blockMap->rect.width) {
                if (mask.y < map.blockMap[y].rect.y + map.blockMap->rect.height &&
                    mask.y > map.blockMap[y].rect.y) { //Deteccion colision arriba
                    //no chocamos contra nada
                    mask.y = map.blockMap[y].rect.y + map.blockMap->rect.height;
                    //chocamos contra algo
                    player.jumping = 0;
                } else if (mask.y + mask.width > map.blockMap[y].rect.y && mask.y < map.blockMap[y].rect.y) {
                    //tocamos piso
                    mask.y = map.blockMap[y].rect.y - map.blockMap->rect.height;
                    //quitamos velocidad de salto al caer en piso
                    player.jumping = 0;
                }
            }
        }
    }
}
