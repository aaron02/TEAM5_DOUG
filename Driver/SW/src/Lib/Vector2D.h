/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Defnies.h"

class Vector2D
{
public:
    // Constructors
    Vector2D(double X, double Y) : x(X), y(Y) 
    {
        genAngles();
    }

    Vector2D() = default;

    void changeCoords(float posX, float posY)
    {
        x = posX;
        y = posY;

        genAngles();
    }

    void genComplimentary() 
    {
        x = 0.0f;
        y = 0.0f;

        x = R * cos(theta);
        y = R * sin(theta);
    }

    void genMagnitude()
    {
        R = sqrt(pow(x, 2) + pow(y, 2));
    }

    void genAngles()
    {
        genMagnitude();

        if (x == 0 && y == 0) 
        {
            theta = 0;
        } 
        else 
        {

            if (x == 0) 
            {

                theta = PI / 2;

                if (y < 0) 
                {
                    theta *= -1;
                }
            }
            else
            {

                theta = atan(y / x);

                if (x < 0) 
                {

                    if (y >= 0) 
                    {

                        theta += PI;
                    } 
                    else if (y < 0) 
                    {
                        theta -= PI;
                    } 
                    else 
                    {

                        theta = PI;
                    }
                }
            }
        }
    }

    double getMagnitude() 
    {
        return R;
    }

    double getTheta() 
    {
        return theta;
    }

    double getX() 
    {
        return x;
    }

    double getY() 
    {
        return y;
    }

    void rotate(double radians) 
    {
        theta = std::fmod((theta + radians), (2 * PI));

        genComplimentary();
    }

    void add(Vector2D* term_two) 
    {
        x += term_two->getX();
        y += term_two->getY();

        genAngles();
    }

protected:
    double x;
    double y;
    double R;
    double theta;
};