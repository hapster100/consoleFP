#include "vector2d.cpp"

template <typename T> class Matrix 
{

private:
  Vector2d<int> size;
  T* matrix;

public:
  Matrix(Vector2d<int> initSize)
  {
    size = initSize;
    matrix = new T[size.getX() * size.getY()];
  }

  void SetMatrixElement(Vector2d<int> position, T element)
  {
    matrix[position.getX() + position.getY() * size.getX()] = element;
  }

  T GetElement(Vector2d<int> position)
  {
    return matrix[position.getX() + position.getY() * size.getX()];
  }

  Vector2d<int> getSize()
  {
    return size;
  }

  void Fill(T filler)
  {
    for (int i = 0; i < size.getX(); i++)
    {
      for (int j = 0; j < size.getY(); j++)
      {
        SetMatrixElement(Vector2d<int>(i, j), filler);
      }
    }
  }

};
