#include <vector>
#include <iostream>
#include <utility>
#include <algorithm>
#include <ncurses.h>

#include "matrix.cpp"

void setPanel(Vector2d<int> position, Matrix<char> panel)
{
  for (int i = 0; i < panel.getSize().getX(); i++)
  {
    for (int j = 0; j < panel.getSize().getY(); j++)
    {
      int xOffset = i + position.getX();
      int yOffset = j + position.getY();

      Vector2d<int> positionInPanel = Vector2d<int>(i, j);
      
      mvaddch(xOffset, yOffset, panel.GetElement(positionInPanel));
    }
  }
}

void setScreen(std::vector<std::pair<int, bool>> distances, Vector2d<int> size)
{
  int screenWidth = size.getX();
  int screenHeigth = size.getY();

  for (int i = 0; i < screenWidth; i++)
  {
    int distance = distances[i].first;
    int isCorner = distances[i].second;
    
    int topWall = screenHeigth/2 - screenHeigth * 10.0f / distance;
    int bottomWall = screenHeigth - topWall;
    
    if (bottomWall < screenHeigth/2)
    {
      bottomWall = screenHeigth/2;
    }

    for (int j = 0; j < size.getY(); j++)
    {
      if(j > topWall && j < bottomWall) 
      { 
        if(isCorner)
        {
          attrset(COLOR_PAIR(50 + 100));
          mvprintw(j, i, "\u2592");
          attroff(COLOR_PAIR(50 + 100));
        }
        else
        {
          attrset(COLOR_PAIR(50 + distance));
          mvprintw(j, i, " ");
          attroff(COLOR_PAIR(50 + distance));
        }

      }
      else if (j >= bottomWall) 
      {
        int floorDistance = distance * (screenHeigth - j) / (screenHeigth - bottomWall);

        attrset(COLOR_PAIR(50 + floorDistance) | A_REVERSE);
        
        if (floorDistance > 90)
        {
          mvprintw(j, i, " ");
        }
        else if (floorDistance > 75)
        {
          mvprintw(j, i, ".");
        }
        else if (floorDistance > 50)
        {
          mvprintw(j, i, "-");
        }
        else if (floorDistance > 20)
        {
          mvprintw(j, i, "=");
        }
        else
        {
          mvprintw(j, i, "#");
        }

        attroff(COLOR_PAIR(50 + floorDistance) | A_REVERSE);
      }
      else
      {
        mvprintw(j, i, " ");
      }
    
    }
    
  }
  
}

std::vector<Vector2d<float>> getNRayDirections(Vector2d<float> centerDirection, float angle, int n)
{
  std::vector<Vector2d<float>> directions = std::vector<Vector2d<float>>(n);
  for (int i = 0; i < n; i++)
  {
    float dAngle = -angle / 2 + angle * i / n;
    directions[i] = Vector2d<float>::Rotate(centerDirection, dAngle);
  }

  return directions;
  
}


template<typename T> bool isVectorInBox(Vector2d<T> vec, Vector2d<T> corner, Vector2d<T> size)
{
  return vec.getX() >= corner.getX() 
    && vec.getX() < corner.getX() + size.getX() 
    && vec.getY() >= corner.getY() 
    && vec.getY() < corner.getY() + size.getY();
}

std::vector<std::pair<int, bool>> viewDistances(
  Vector2d<int> size, 
  Matrix<char> map, 
  Vector2d<float> position, 
  Vector2d<float> direction, 
  float viewAngle)
{
  int viewWidth = size.getX();

  std::vector<Vector2d<float>> viewDirections = getNRayDirections(direction, viewAngle, viewWidth);
  std::vector<int> distances = std::vector<int>(viewWidth);
  std::vector<bool> corners = std::vector<bool>(viewWidth);

  const float distanceStep = 0.15f;
  const int maxDistance = 100;

  for (int i = 0; i < viewWidth; i++)
  {
    distances[i] = 0;
    corners[i] = false;

    while(distances[i] < maxDistance)
    {
      Vector2d<float> ray = viewDirections[i] * distances[i] * distanceStep * Vector2d<float>::Dot(viewDirections[i], direction);
      Vector2d<int> rayOnMap = Vector2d<int>(position.getX() + ray.getX(), position.getY() + ray.getY()); 

      if(isVectorInBox(rayOnMap, Vector2d<int>(0, 0), map.getSize()) && map.GetElement(rayOnMap) == '#')
      {
        std::vector<std::pair<float, float>> cornerRays;
        for (int tileX = 0; tileX < 2; tileX++)
        {
          for (int tileY = 0; tileY < 2; tileY++)
          {
            Vector2d<float> cornerRay = Vector2d<float>(rayOnMap.getX() + tileX, rayOnMap.getY() + tileY) - position;
            float distanceToCorner = sqrtf(powf(cornerRay.getX(), 2) + powf(cornerRay.getY(), 2));
            float dot = Vector2d<float>::Dot(cornerRay * (1 / distanceToCorner), viewDirections[i]);

            cornerRays.push_back(std::make_pair(distanceToCorner, dot));
          } 
        }
        
        std::sort(cornerRays.begin(), cornerRays.end(), [](std::pair<float,float> p1, std::pair<float, float> p2){return p1.first < p2.first;});

        float cornerViewCos = 0.0075f;
        if(acos(cornerRays[0].second) < cornerViewCos) corners[i] = true;
        if(acos(cornerRays[1].second) < cornerViewCos) corners[i] = true;
        if(acos(cornerRays[2].second) < cornerViewCos) corners[i] = true;

        break;
      }

      distances[i]++;
    }
  }

  std::vector<std::pair<int, bool>> distancesWithCorners = std::vector<std::pair<int, bool>>(viewWidth);

  for (int i = 0; i < size.getX(); i++)
  {
    distancesWithCorners[i] =  std::pair<int, bool>(distances[i], corners[i]);
  }
  
  return distancesWithCorners;
  
}

Matrix<char> initMap()
{
  Vector2d<int> mapSize = Vector2d<int>(20, 20);
  Matrix<char> map = Matrix<char>(mapSize);

  const char** map_content = new const char*[mapSize.getY()];
  map_content[0] =  "####################";
  map_content[1] =  "#......#...........#";
  map_content[2] =  "#......#...........#";
  map_content[3] =  "#......#...........#";
  map_content[4] =  "#......#.....#.....#";
  map_content[5] =  "#............#.....#";
  map_content[6] =  "#............#.....#";
  map_content[7] =  "##############.....#";
  map_content[8] =  "#..................#";
  map_content[9] =  "#..................#";
  map_content[10] = "#..................#";
  map_content[11] = "#............##....#";
  map_content[12] = "#..................#";
  map_content[13] = "#..................#";
  map_content[14] = "#........##........#";
  map_content[15] = "#........##........#";
  map_content[16] = "#..................#";
  map_content[17] = "#..................#";
  map_content[18] = "#..................#";
  map_content[19] = "####################";

  for (int i = 0; i < mapSize.getX(); i++)
  {
    for (int j = 0; j < mapSize.getY(); j++)
    {
      map.SetMatrixElement(Vector2d<int>(i, j), map_content[i][j]);
    }
  }

  return map;
}


int main()
{
  setlocale(LC_ALL, "");
  initscr();
  curs_set(0);
  noecho();
  keypad(stdscr, true);
  start_color();
  init_color(COLOR_BLACK, 0, 0, 0);
  
  for (int i = 0; i <= 100; i++)
  {
    int color = 1000 - i * 10;
    init_color(50 + i, color, color, color);
    init_pair(50 + i, COLOR_BLACK, 50 + i);
  }
  
  Vector2d<float> playerPosition = Vector2d<float>(1.5f, 1.5f);
  Vector2d<float> playerDirection = Vector2d<float>::Normalize(Vector2d<float>(1.0f, 1.0f));

  Matrix<char> map = initMap();
  
  bool noExit = true;

  const float viewAngle = M_PI / 4;

  while(noExit)
  {

    Vector2d<int> screenSize = Vector2d<int>(COLS, LINES);

    std::vector<std::pair<int, bool>> distancesAndCorners = viewDistances(screenSize, map, playerPosition, playerDirection, viewAngle);

    setScreen(distancesAndCorners, screenSize);
    
    Vector2d<int> mapPanelPositon = Vector2d<int>(1, 1); 
    setPanel(mapPanelPositon, map);

    Vector2d<int> playerPositionOnMap = Vector2d<int>(playerPosition.getX(), playerPosition.getY());
    
    Vector2d<int> playerLabelOnScreen = mapPanelPositon + playerPositionOnMap;
    Vector2d<int> playerDirectionLabel = playerLabelOnScreen + Vector2d<int>(playerDirection.getX() * 1.5f, playerDirection.getY() * 1.5f);
    mvaddch(playerLabelOnScreen.getX(), playerLabelOnScreen.getY(), '@');
    mvaddch(playerDirectionLabel.getX(), playerDirectionLabel.getY(), '+');

    mvprintw(2, 25, "size distanceMap: %dx%d", COLS, LINES);

    
    refresh();

    const float playerStep = 0.25f;
    const float diretionStep = 10.0f;

    switch (getch())
    {
      case KEY_DOWN:
        playerPosition -= playerDirection * playerStep;
        if(map.GetElement(Vector2d<int>(playerPosition.getX(), playerPosition.getY())) == '#')
          playerPosition += playerDirection * playerStep;
        break;
      case KEY_UP:
        playerPosition += playerDirection * playerStep;
        if(map.GetElement(Vector2d<int>(playerPosition.getX(), playerPosition.getY())) == '#')
          playerPosition -= playerDirection * playerStep;
        break;
      case KEY_RIGHT:
        playerDirection = Vector2d<float>::Rotate(playerDirection, diretionStep * M_PI / 180.0f);
        break;
      case KEY_LEFT:
        playerDirection = Vector2d<float>::Rotate(playerDirection, -diretionStep * M_PI / 180.0f);
        break;
      case 'q':
        noExit = false;
        break;
    }
  }

  endwin();

  return 0;
}