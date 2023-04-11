/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Defnies.h"

class Vector2D
{
protected:
    std::vector<double> components;
    double R;
    double theta;

public:
    Vector2D(double comp1, double comp2)
    {
        components = { comp1, comp2 };
    }

    Vector2D(std::vector<double> comp, double magnitude, double theta)
    {
        components = comp;
        R = magnitude;
        theta = theta;
    }

    void genComplimentary() 
    {
        components = { 0.0f, 0.0f };

        components[0] = R * cos(theta);
        components[1] = R * sin(theta);
    }

    void genMagnitude()
    {
        R = sqrt(pow(components[0], 2) + pow(components[1], 2));
    }

    void genAngles()
    {
        genMagnitude();

        if (components[0] == 0 && components[1] == 0) 
        {
            theta = 0;
        } 
        else 
        {

            if (components[0] == 0) 
            {

                theta = PI / 2;

                if (components[1] < 0) 
                {
                    theta *= -1;
                }
            }
            else
            {

                theta = atan(components[1] / components[0]);

                if (components[0] < 0) 
                {

                    if (components[1] >= 0) 
                    {

                        theta += PI;
                    } 
                    else if (components[1] < 0) 
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

    double getMagnitude() {

        return R;
    }

    double getTheta() {

        return theta;
    }

    std::vector<double> getComponents() {

        return components;
    }

    void rotate(double radians) 
    {
        double test;
        double test2;

        theta = std::fmod((theta + radians), (2 * PI));

        genComplimentary();
    }

    void add(Vector2D term_two) 
    {

        std::vector<double> two_comp = term_two.getComponents();

        components[0] += two_comp[0];
        components[1] += two_comp[1];

        genAngles();
    }

    void sub(Vector2D term_two) 
    {
        add(invert(term_two));
    }

    static Vector2D invert(Vector2D term_two) 
    {
        Vector2D* iTwo = new Vector2D(term_two.getComponents(), term_two.getMagnitude(), term_two.getTheta());
        iTwo->scale(-1);

        return *iTwo;
    }

    void scale(double scalar) 
    {
        components[0] *= scalar;
        components[1] *= scalar;
    }
};