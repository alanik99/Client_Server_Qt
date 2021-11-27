#ifndef CALCULATOR_H
#define CALCULATOR_H

struct CalculationResult
{
    float v;
    float tetta;
    float y;
    float x;
};

class Calculator
{
public:
    Calculator();

    CalculationResult calculate(float d, float dt, float v0, float y0, float cx0,
               float A, float cya, float teta, float m0, float mc,
               float tet, float tr, float aero);
};

#endif // CALCULATOR_H
