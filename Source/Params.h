#pragma once

enum Slope
{
    Slope12,
    Slope24,
    Slope36,
    Slope48
};

enum class FilterToChoose
{
    PeakFilter,
    LowShelfFilter,
    HighShelfFilter,
    NotchFilter
};

struct EqChain {
    float HiPassFreq{ 0 }, lowPassFreq{ 0 };
    float filterFreq{ 0 }, gainInDb{ 0 }, filterQuality{ 1.f };
    float filterFreq2{ 0 }, gainInDb2{ 0 }, filterQuality2{ 1.f };
    float filterFreq3{ 0 }, gainInDb3{ 0 }, filterQuality3{ 1.f };
    Slope HiPassSlope{ Slope::Slope12 }, lowPassSlope{ Slope::Slope12 };
    FilterToChoose Filter1choice{ FilterToChoose::PeakFilter }, Filter2choice{ FilterToChoose::PeakFilter }, Filter3choice{ FilterToChoose::PeakFilter };
    float totalGain{ 0 };
    bool hfpBypass{ true }, filter1bypass{ false }, filter2bypass{ false }, filter3bypass{ false }, lpfBypass{ true };
};

struct NoiseGateChain 
{
    float threshold{ -48 };
    float ratio{ 1 };
    float attack{ 0 };
    float release{ 0 };
    bool noiseGateBypass{ false };
};

struct CompressorChain
{
    float threshold{ -48 };
    float ratio{ 1 };
    float attack{ 0 };
    float release{ 0 };
    bool compressorBypass{ false };
};

enum Chainpositons
{
    NoiseGate,
    Compressor,
    HiPass,
    Filter1,
    Filter2,
    Filter3,
    LowPass,
    Gain
};