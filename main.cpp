//
//  main.cpp
//  Snake
//
//  Created by Mikhail on 02.05.15.
//  Copyright (c) 2015 Mikhail. All rights reserved.
//

#include <iostream>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <time.h>
#include "math.h"

#define WIDTH 25
#define HEIGHT 15
#define MAX_BLOCKS 1000

void reverse(char s[])
{
    int i, j;
    char c;
    
    for (i = 0, j = (int)strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void itoa(int n, char s[])
{
    int i;
    i = 0;
    do {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    s[i] = '\0';
    reverse(s);
}

int get_bin_count(int n){
    int count = 0;
    while (n != 0){
        n = n / 2;
        count ++;
    }
    return count;
}

class Settings{
public:
    int delay;
    float scale;
    int WinScale;
    int WinPosX;
    int WinPosY;
    int StartPosX;
    int StartPosY;
    int StartSize;
    Settings(){
        StartPosX = WIDTH/2;
        StartPosY = HEIGHT/2;
        StartSize = 1;
        delay = 50;
        scale = 1;
        WinScale = 100;
        WinPosX = 0;
        WinPosY = 0;
    }
    void init();
};
Settings settings;

class Text{
    public:
        char digit[5];
        char digit0[2] = "0";
        char digit1[2] = "1";
        char score[10] = "Score";
        void output(float x, float y, float r, float g, float b, char* string);
    
};
Text text;

void Text :: output(float x, float y, float r, float g, float b, char* string)
{
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    int len, i;
    len = (int)strlen(string);
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
    }
}

class Block {
public:
    int x;
    int y;
    int view;
    Block(){
        x = -1;
        y = -1;
    }
    void draw(int i);
    
  
};
Block blocks[MAX_BLOCKS];

void Block :: draw(int i){
    glColor3f(0, 1, 0);
    //we say opengl that we want to create closed curve
    glBegin(GL_LINE_LOOP);
        glVertex2f(x*settings.scale, y*settings.scale);
        glVertex2f(x*settings.scale + settings.scale, y*settings.scale);
        glVertex2f(x*settings.scale + settings.scale, y*settings.scale + settings.scale);
        glVertex2f(x*settings.scale, y*settings.scale + settings.scale);
    glEnd();
    //just magic
    if (i != 0) {
        if (view == 1)
            text.output(x + 0.4, y + 0.4, 1, 1, 1, text.digit1);
        else
            text.output(x + 0.4, y + 0.4, 1, 1, 1, text.digit0);
    }
}

class Food{
public:
    int x0;
    int y0;
    int x1;
    int y1;
    Food(){
        x0 = -1;
        y0 = -1;
        x1 = -1;
        y1 = -1;
    }
    void draw_0();
    void draw_1();
    void spawn();
};
Food food;

class Snake{
public:
    int direction;
    int size;
    bool turn;
    Snake(){
        turn = false;
        size = settings.StartSize;
        direction = GLUT_KEY_DOWN;
    }
    void drawSnake();
    void move();
    void die();
    void eat();
    
};
Snake snake;

class Numbers{
public:
    int bin_count;
    int count;
    int score;
    int goal;
    int in_a_row;
    int complexity;
    bool dig[16];
    Numbers(){
        bin_count = 0;
        count = -1;
        score = 0;
        in_a_row = 0;
        complexity = 10;
    }
    void randgoal();
    int compare();
    void decision();
    void dig_in_bin(int n);
    void In_a_row();
};
Numbers numbers;

void Numbers::  dig_in_bin(int n){
    for (int i = get_bin_count(n) - 1; i >= 0;i--){
         dig[i] = n % 2;
         n /= 2;
    }
}
 
 
 void Numbers::randgoal(){
     goal = rand() % complexity + 1;
     bin_count = get_bin_count(goal);
     itoa(goal, text.digit);
 }

void Numbers::decision(){
     int i = compare();
     if ( i == 0){
         count = -1;
         in_a_row = 0;
         itoa(score, text.score);
         randgoal();
     }
     if (i == 1){
         count = -1;
         snake.size -= get_bin_count(goal);
         score += goal;
         In_a_row();
         itoa(score, text.score);
         randgoal();
     }
}

 int Numbers::compare(){
     dig_in_bin(goal);
     if (dig[count] != blocks[snake.size].view){
         return 0;
     }
     if (snake.size >= bin_count){
         int i = 0;
         int counter = 0;
         while (i < bin_count) {
         if (dig[i] == blocks[snake.size - bin_count + 1 + i].view )
             counter++;
         i++;
         }
         if (counter == bin_count)
             return 1;
     }
     return -1;
 }
void Numbers::In_a_row(){
    in_a_row++;
    if (in_a_row == 3) {
        complexity += 10;
        in_a_row = 0;
    }
}
void Snake::drawSnake(){
    for (int i = 0; i < size; i++) {
        blocks[i].draw(i);
    }
}

void Snake::move(){
    //this loop transport our body from tail to head
    for (int i = size; i > 0; i--) {
        blocks[i].x = blocks[i-1].x;
        blocks[i].y = blocks[i-1].y;
    }
    //change dirrection of head
    switch (direction) {
        case GLUT_KEY_RIGHT:
            blocks[0].x++;
            break;
        case GLUT_KEY_LEFT:
            blocks[0].x--;
            break;
        case GLUT_KEY_UP:
            blocks[0].y++;
            break;
        case GLUT_KEY_DOWN:
            blocks[0].y--;
            break;
    }
    //transporting our snake if she get out from the screen
    if (blocks[0].x >= WIDTH)
        blocks[0].x -= WIDTH;
    if (blocks[0].x < 0)
        blocks[0].x += WIDTH;
    if (blocks[0].y >= HEIGHT)
        blocks[0].y -= HEIGHT;
    if (blocks[0].y < 0)
        blocks[0].y += HEIGHT;
}

void Snake::eat(){
    if ((blocks[0].x == food.x0) && (blocks[0].y == food.y0)){
        //set view of our blocks
        blocks[size].view = 0;
        numbers.count++;
        numbers.decision();
        size++;
        food.spawn();
    }
    if ((blocks[0].x == food.x1) && (blocks[0].y == food.y1)){
        blocks[size].view = 1;
        numbers.count++;
        numbers.decision();
        size++;
        food.spawn();
    }
}


void Snake::die(){
    for (int i = 1; i < size; i++) {
        if ((blocks[0].x == blocks[i].x) && (blocks[0].y) == blocks[i].y) {
            exit(0);
        }
    }
}

void Settings :: init(){
    blocks[0].x = StartPosX;
    blocks[0].y = StartPosY;
    numbers.randgoal();
}

void Food::draw_0(){
    glColor3f(1,1,1);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x0 * settings.scale,y0 * settings.scale);
        glVertex2f(x0 * settings.scale + settings.scale,y0 * settings.scale);
        glVertex2f(x0 * settings.scale + settings.scale,y0 * settings.scale + settings.scale);
        glVertex2f(x0 * settings.scale,y0 * settings.scale + settings.scale);
    glEnd();
    text.output(x0 + 0.4, y0 + 0.4, 1, 1, 1, text.digit0);
}

void Food::draw_1(){
    glColor3f(1,1,1);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x1*settings.scale,y1*settings.scale);
        glVertex2f(x1*settings.scale + settings.scale,y1*settings.scale);
        glVertex2f(x1*settings.scale + settings.scale,y1*settings.scale + settings.scale);
        glVertex2f(x1*settings.scale,y1*settings.scale + settings.scale);
    glEnd();
    text.output(x1 + 0.4, y1 + 0.4, 1, 1, 1, text.digit1);
}

void Food::spawn(){
    bool same;
    //this loop protect us from creating food on the snake or other food
    do{
        same = false;
        x0 = rand()%WIDTH;
        y0 = rand()%HEIGHT;
        x1 = (x0 * 2) % WIDTH;
        y1 = (x0 * 2) % HEIGHT;
        for (int i =0; i < snake.size; i++) {
            if ((blocks[i].x == x0) && (blocks[i].y == y0) && (x1 == x0) && (y1 == y0))
                same = true;
        }
    }while(same);
}

void init(){ 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,WIDTH,0,HEIGHT,0,1);
}


void keyboard(int key,int x, int y){
    switch(key){
        case GLUT_KEY_LEFT :
            if((snake.direction != GLUT_KEY_RIGHT)&&(snake.turn))
                snake.direction = GLUT_KEY_LEFT;
            snake.turn = false;
            break;
        case GLUT_KEY_RIGHT :
            if((snake.direction != GLUT_KEY_LEFT)&&(snake.turn))
                snake.direction = GLUT_KEY_RIGHT;
            snake.turn = false;
            break;
        case GLUT_KEY_UP :
            if((snake.direction != GLUT_KEY_DOWN)&&(snake.turn))
                snake.direction = GLUT_KEY_UP;
            snake.turn = false;
            break;
        case GLUT_KEY_DOWN :
            if((snake.direction != GLUT_KEY_UP)&&(snake.turn))
                snake.direction = GLUT_KEY_DOWN;
            snake.turn = false;
            break;
    }
}

void timer(int){
    snake.turn = true;
    snake.move();
    snake.eat();
    snake.die();
    glutPostRedisplay();
    glutTimerFunc(settings.delay, timer, 0);
}

void rendering(){
    //clear buffer of collor
    glClear(GL_COLOR_BUFFER_BIT);
    //message to openGL that we want to draw square
    glBegin(GL_QUADS);
        snake.drawSnake();
        food.draw_0();
        food.draw_1();
        text.output(settings.scale * WIDTH - 2,settings.scale * HEIGHT - 1, 1, 1, 0.5, text.score);
        text.output(settings.scale * WIDTH / 2,settings.scale * HEIGHT - 1, 1, 0, 0, text.digit);
    glEnd();
    glutSwapBuffers();
}

int main (int argc, char ** argv){
    settings.init();
    glutInit(&argc, argv);
    food.spawn();
    srand((int)time(0));
    //Creating window
    glutInitWindowPosition(settings.WinPosX, settings.WinPosY);
    glutInitWindowSize(WIDTH*settings.WinScale, HEIGHT*settings.WinScale);
    glutInitDisplayMode(GLUT_RGB);
    glutCreateWindow("Oыh My GOD! Snake!");
    //
    glutDisplayFunc(rendering);
    glutTimerFunc(settings.delay,timer, 0);
    glutSpecialFunc(keyboard);
    //initialize starting variables
    init();
    //enter the main loop
    glutMainLoop();
    return 0;
}














