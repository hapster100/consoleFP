#include <cmath>

template <typename T> class Vector2d 
{
  private:
    T x, y;

  public:
    Vector2d()
    {
      x = 0;
      y = 0;
    }
    Vector2d(T x0, T y0)
    {
      x = x0;
      y = y0;
    }

    T getX()
    {
      return x;
    }

    T getY()
    {
      return y;
    }

    void operator+=(Vector2d<T> vec2d)
    {
      x += vec2d.x;
      y += vec2d.y;
    }

    void operator-=(Vector2d<T> vec2d)
    {
      x -= vec2d.x;
      y -= vec2d.y;
    }

    Vector2d<T> operator+(Vector2d<T> v)
    {
      return Vector2d<T>(x + v.x, y + v.y);
    }

    Vector2d<T> operator-(Vector2d<T> v)
    {
      return Vector2d<T>(x - v.x, y - v.y);
    }

    Vector2d<T> operator*=(T scal)
    {
      x *= scal;
      y *= scal;
    }

    Vector2d<T> operator*(T scal)
    {
      return Vector2d<T>(x * scal, y * scal);
    }

    T Magnitude()
    {
      return sqrt(x*x + y*y);
    }

    Vector2d<T> static Normalize(Vector2d<T> vec)
    {
      if(vec.x == 0 && vec.y == 0)
      {
        return Vector2d<T>(0,0);
      }

      float length = sqrt(vec.x * vec.x + vec.y * vec.y);
      return Vector2d<float>(vec.x / length, vec.y / length);
    }

    Vector2d<T> static Rotate(Vector2d<T> vec, float rad)
    {
      if(vec.isZero())
      {
        return Vector2d<T>(0.0f, 0.0f);
      }

      float rotateX = vec.x * cos(rad) + vec.y * sin(rad);
      float rotateY = -vec.x * sin(rad) + vec.y * cos(rad);

      return Vector2d<float>(rotateX, rotateY);
    }

    T static Dot(Vector2d<T> v1, Vector2d<T> v2)
    {
      return v1.x * v2.x + v1.y * v2.y;
    }

    bool isZero() {
      return x == 0 && y == 0;
    }

};
