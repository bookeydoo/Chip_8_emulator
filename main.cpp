
#include<iostream>
#include<raylib.h>
#include"chip.hpp"

//TODO later maybe for better organiziation 
int main(int argc, char** argv)
{
    Chip8 cpu;
    
    uint8_t msg_flag = 0; //this is reuseable to not waste memory
    int window_length = 0;
    int window_height = 0;


    if (argc < 2) {
        std::cerr << "sth wrong occured please input the rom and try again";
    }
    std::cout << "please select the size u want from the following:\n\t 1.800x500 \t 2.1200x800  ";
    uint8_t choice;
    std::cin >> choice;
    if (choice == 1) {
        window_length = 800;
        window_height = 500;
    }
    else {
        window_length = 1200;
        window_height = 800;
    }
    InitWindow(window_length,window_height,"Chip8 emulator");
 
   


    //////////////////////////////////
    //HERE are the vars used for the intro animation
    /////////////////////////////////
    int logoposition_x = window_length / 2;
    int logoposition_y = window_height / 2;

    int framesCounter = 0;
    int toplogoRecwidth= 16;
    int leftlogoRecHeight = 16;
    
    int bottomlogoRecWidth = 16;
    int rightlogoRecHeight = 16;

    int lettersCount=0;
    int state = -1; //for finite state machine
    float alpha = 1.0; //for fading(idk what this means )

    SetTargetFPS(60);
    
    while (!WindowShouldClose()) {
        //the logo stuff starts here  for checks first
          // Update
        //----------------------------------------------------------------------------------
        if (state == 0)                 // State 0: Small box blinking
        {
            framesCounter++;

            if (framesCounter >= 120)
            {
                state = 1;
                framesCounter = 0;      // Reset counter... will be used later...
            }
        }
        else if (state == 1)            // State 1: Top and left bars growing
        {
            toplogoRecwidth += 4;
            leftlogoRecHeight += 4;

            if (toplogoRecwidth >= 256) state = 2;
        }
        else if (state == 2)            // State 2: Bottom and right bars growing
        {
            bottomlogoRecWidth += 4;
            rightlogoRecHeight += 4;

            if (bottomlogoRecWidth >= 256) state = 3;
        }
        else if (state == 3)            // State 3: Letters appearing (one by one)
        {
            framesCounter++;

            if (framesCounter / 12)       // Every 12 frames, one more letter!
            {
                lettersCount++;
                framesCounter = 0;
            }

            if (lettersCount >= 10)     // When all letters have appeared, just fade out everything
            {
                alpha -= 0.02f;

                if (alpha <= 0.0f)
                {
                    alpha = 0.0f;
                    state = 4;
                }
            }
        }
        else if (state == 4)            // State 4: Reset and Replay
        {
            if (IsKeyPressed(KEY_R))
            {
                framesCounter = 0;
                lettersCount = 0;

                toplogoRecwidth = 16;
                leftlogoRecHeight = 16;

                bottomlogoRecWidth = 16;
                rightlogoRecHeight = 16;

                alpha = 1.0f;
                state = 0;          // Return to State 0
            }
        }


        BeginDrawing();
        ClearBackground(BLACK);
        DrawFPS(window_length-200, 0);

        if (state == -1) {
            DrawText("Welcome to my humble emulator ", (window_length / 2)-100, window_height/2  , 30, BLACK);
        }
        framesCounter++;   
        if (framesCounter == 150 && state== -1) {
            framesCounter = 0;//reset
            state++;
        }
        ClearBackground(RAYWHITE);

        if (state == 0)
        {
            if ((framesCounter / 15) % 2) DrawRectangle(logoposition_x, logoposition_y, 16, 16, BLACK);
        }
        else if (state == 1)
        {
            DrawRectangle(logoposition_x, logoposition_y, toplogoRecwidth, 16, BLACK);
            DrawRectangle(logoposition_x, logoposition_y, 16, leftlogoRecHeight, BLACK);
        }
        else if (state == 2)
        {
            DrawRectangle(logoposition_x, logoposition_y, toplogoRecwidth, 16, BLACK);
            DrawRectangle(logoposition_x, logoposition_y, 16, leftlogoRecHeight, BLACK);

            DrawRectangle(logoposition_x + 240, logoposition_y, 16, rightlogoRecHeight, BLACK);
            DrawRectangle(logoposition_x, logoposition_y + 240, bottomlogoRecWidth, 16, BLACK);
        }
        else if (state == 3)
        {
            DrawRectangle(logoposition_x, logoposition_y, toplogoRecwidth, 16, Fade(BLACK, alpha));
            DrawRectangle(logoposition_x, logoposition_y + 16, 16, leftlogoRecHeight - 32, Fade(BLACK, alpha));

            DrawRectangle(logoposition_x + 240, logoposition_y + 16, 16, rightlogoRecHeight - 32, Fade(BLACK, alpha));
            DrawRectangle(logoposition_x, logoposition_y + 240, bottomlogoRecWidth, 16, Fade(BLACK, alpha));

            DrawRectangle(GetScreenWidth() / 2 - 112, GetScreenHeight() / 2 - 112, 224, 224, Fade(RAYWHITE, alpha));

            DrawText(TextSubtext("raylib", 0, lettersCount), GetScreenWidth() / 2 - 44, GetScreenHeight() / 2 + 48, 50, Fade(BLACK, alpha));
        }

        EndDrawing();
    }


    return 0;
}
