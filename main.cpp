#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <iostream>
///////////////////////////////////////////////////////////////////////////////////////////////

class Tile;

class Tile_Type
{
public:

    bool passable;
    sf::Sprite* sprite;
    void loadSprite(sf::Texture& texture, sf::IntRect rectangle)
    {
        sprite = new sf::Sprite;
        sprite->setTexture(texture);
        sprite->setTextureRect(rectangle);
    }

    void loadSprite(sf::Sprite* sprite)
    {
        this->sprite= sprite;
    }

    friend Tile;
} wall, floor, ending;
////////////////////////////////////////////////////////////////////////////////////////////////

class Tile
{
    Tile_Type* type;
public:

    int x, y;
    void draw(sf::RenderWindow& window)
    {
        type->sprite->setPosition(x*16,y*16);
        window.draw(*(type->sprite));
    }
    Tile(int x, int y, Tile_Type* type)
    {
        this-> x= x;
        this-> y= y;
        this-> type= type;
    }
    Tile()
    {
        x=0;
        y=0;
    }
    bool is_passable()
    {

        return type->passable;
    }
};
///////////////////////////////////////////////////////////////////////////////////////////////
class Level;

class Avatar
{
public:

    int avx;
    int avy;

    int dx;
    int dy;

    bool moving;

    int direction;
    int fram;

    float offset= 0;
    float speed= 50;

    sf::Sprite avt[4][5];

    sf::Clock moves_timer;
    sf::Clock avt_clock;



    void set_avatar_direction_and_fram(sf::Texture& texture )
    {
        for(direction=0; direction < 4; direction++)
        {
            for(fram=0; fram < 5; fram++)
            {
                avt[direction][fram].setTexture(texture);
                avt[direction][fram].setTextureRect(sf::IntRect(12+175*direction+ 35*fram,
                                                                772,18,20));
                avt[direction][fram].setOrigin(0,2);
            }
        }
        direction= 2;
        fram= 0;
    }

    void draw(sf::RenderWindow& window)
    {
            if(moving)
            {
                avt[direction][fram].setPosition(avx*16+offset*dx,avy*16+offset*dy);
                window.draw(avt[direction][fram]);
            }
            else
            {
                avt[direction][0].setPosition(avx*16,avy*16);
                window.draw(avt[direction][0]);
            }
        }

        void update()
        {
            if(avt_clock.getElapsedTime().asSeconds()>0.5)
                {
                    fram=(fram + 1) %5;
                    avt_clock.restart();
                }

            if (moving)
                {
                    offset+= moves_timer.getElapsedTime().asSeconds()*speed;
                    if(offset >=16)
                    {
                        avy+= dy;
                        avx+= dx;
                        moving= 0;
                        offset= 0;
                    }
                }
        }


    friend Level;

};

////////////////////////////////////////////////////////////////////////////////////////////////
class Level
{
public:

    Avatar avatar;
    int width;
    int height;
    int enx;
    int eny;
    Tile maze[100][100];

    void load_level (std::string& level_name, std::ifstream& level_ex)
    {
        std::ifstream level_file;
        level_file.open(level_name.c_str());
        level_file >> width >>height;
        for (int y=0; y<height; y++)
        {
            for (int x=0; x<width; x++)
            {
                char maze_char;
                level_file>> maze_char;
                switch(maze_char)
                {
                case '@':
                    avatar.avx = x;
                    avatar.avy = y;
                    maze[x][y] = Tile(x,y,&floor);
                    break;

                case '!':
                    enx=x;
                    eny=y;
                    maze[x][y] = Tile(x,y,&ending);
                    break;

                case '.':
                    maze[x][y] = Tile(x,y,&floor);
                    break;

                case '#':
                    maze[x][y] = Tile(x,y,&wall);
                    break;
                }
            }
        }
        level_file.close();
    }

    bool moves (int delta_x, int delta_y)
    {
        if(maze[avatar.avx+ delta_x][avatar.avy + delta_y].is_passable())
            {
                avatar.dy = delta_y;
                avatar.dx = delta_x;
                avatar.moving= 1;
            }
        else
            avatar.moving= 0;
    }

};
////////////////////////////////////////////////////////////////////////////////////////////////
void change_view (sf::View& view, int& width, int& height, sf::RenderWindow& window);
//change the window size
////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    //open window
    sf::RenderWindow window;
    sf::Event event;
    sf::View view;
    Level level;
    int level_num;
    std::ifstream level_ary[4];
    std::string nn[4];
    nn[0] = "maze.txt";
    nn[1] = "maze(1).txt";
    nn[2] = "maze(2).txt";
    nn[3] = "maze(3).txt";

    //lode the maze
    level_num=0;
    level.load_level(nn[level_num],level_ary[level_num]);

    //lode images
    sf::Image dun_ima;
    dun_ima.loadFromFile("dungeon.png");
    sf::Texture dun_tex;
    dun_tex.loadFromImage(dun_ima);


    //Tile_Type wall;
    wall.loadSprite(dun_tex, sf::IntRect(32,40,16,16));
    wall.passable = false;


    //Tile_Type ending;
    ending.loadSprite(dun_tex, sf::IntRect(0,89,16,16));
    ending.passable = true;

    //Tile_Type floor;
    floor.loadSprite(dun_tex,sf::IntRect(208,168,16,16));
    floor.passable = true;

    sf::Clock grass_clock;
    int grass_fram = 0;

    sf::Image chars;
    chars.loadFromFile("chars.png");
    chars.createMaskFromColor(chars.getPixel(11,155));
    sf::Texture chars_tex;
    chars_tex.loadFromImage(chars);


    level.avatar.set_avatar_direction_and_fram(chars_tex);

    window.create(sf::VideoMode(800,590),"Maze");
    change_view(view, level.width, level.height, window);

    while(window.isOpen())
    {
        level.avatar.moves_timer.restart();
        while(window.pollEvent(event))
        {
            if(event.type==sf::Event::Closed)
                window.close();

            if(event.type==sf::Event::Resized)
            {
                view.setSize(event.size.width,event.size.height);
                view.setCenter(event.size.width/2,event.size.height/2);

            }
            if(event.type==sf::Event::KeyPressed)
            {
                switch(event.key.code)
                {
                case sf::Keyboard::Right:
                    level.avatar.direction=0;
                    level.moves(1, 0);
                    break;

                case sf::Keyboard::Left:
                    level.avatar.direction=3;
                    level.moves( -1, 0);
                    break;

                case sf::Keyboard::Up:
                    level.avatar.direction=2;
                    level.moves( 0, -1);
                    break;

                case sf::Keyboard::Down:
                    level.avatar.direction=1;
                    level.moves( 0, 1);
                    break;

                case sf::Keyboard::F12:
                     ++level_num;
                    change_view(view, level.width, level.height, window);
                    break;
                default:
                    break;
                }
           }

       }//event loop

       //output
       window.setView(view);
       window.clear();
       for(int x=0; x < level.width; x++)
       {
           for(int y=0; y<level.height; y++)
           {
               level.maze[x][y].draw(window);
           }
       }
       level.avatar.draw(window);

       window.display();

       //logic

       if(grass_clock.getElapsedTime().asSeconds()>0.5)
       {
            grass_fram=(grass_fram + 1) %2;
            grass_clock.restart();
       }

       level.avatar.update();

        if((level.avatar.avx==level.enx) && (level.avatar.avy==level.eny))
        {
            if(level_num==3)
                window.close();
            else
            {
                ++level_num;
                level.load_level(nn[level_num],level_ary[level_num]);
                change_view(view, level.width, level.height, window);
            }
        }

   }//while(window.isOpen())end

    return EXIT_SUCCESS;
}
/////////////////////////////////////////////////////////////////////////////////////////
void change_view (sf::View& view, int& width, int& height, sf::RenderWindow& window )
{
    view.setSize(width*16, height*16);
    view.setCenter(width*16/2, height*16/2);
    window.setSize(sf::Vector2u(width*16, height*16));
}
